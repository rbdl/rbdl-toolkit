#include "AnimationPlugin.h"
#include <iostream>
#include <clocale>

#include <QFileDialog>
#include <QMessageBox>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "csv_input_sanatiser.hpp"
#include <parser.hpp>

using namespace RigidBodyDynamics::Math;

AnimationPlugin::AnimationPlugin() {
	parentApp = NULL;
}

AnimationPlugin::~AnimationPlugin() {
}

void AnimationPlugin::init(ToolkitApp* app) {
	//save reference to parent ToolkitApp 
	parentApp = app;

	load_file_trigger = new QAction("Load Animation");
	parentApp->addFileAction(load_file_trigger);

	connect(load_file_trigger, SIGNAL(triggered(bool)), this, SLOT(action_load_animation()));

	getCSVSettings();

	QCommandLineOption animation_option( QStringList() << "a" << "animation",
	                                 "Load Animation files <file>", 
	                                 "file"
	                               );
	parentApp->addCmdOption(animation_option, [this](QCommandLineParser& parser){
		auto animation_list = parser.values("animation");

		for (int i=0; i<animation_list.size(); i++) {
			if (i < parentApp->getLoadedModels()->size() ) {
				AnimationModelExtention* ext;
				try {
					ext = this->loadAnimationFile(animation_list[i]);
				} catch (RigidBodyDynamics::Errors::RBDLError& e){
					ToolkitApp::showExceptionDialog(e);
					continue;
				}
				RBDLModelWrapper* model = parentApp->getLoadedModels()->at(i);
				model->addExtention(ext);
				parentApp->getToolkitTimeline()->setMaxTime(ext->getMaxTime());
				loaded_animations.push_back(ext);
			} else {
				std::cout << QString("Animationfile %1 can not be mapped to a model ... Ignoring!").arg(animation_list[i]).toStdString() << std::endl;
			}
		}
	});
}

void AnimationPlugin::getCSVSettings() {
	parentApp->toolkit_settings.beginGroup("FileReaderOptions");

	//seperator setting
	QVariant val = parentApp->toolkit_settings.value("csv.seperator");
	if (val.isNull()) {
		csv_seperator = ',';
		parentApp->toolkit_settings.setValue("csv.seperator", csv_seperator);
	} else {
		//read as int because it is saved as one, otherwise it would not load correctly
		//watch out for overflow -> will silently fail because char is uint8
		csv_seperator = val.toInt();
	}

	//trimming setting
	val = parentApp->toolkit_settings.value("csv.trim");
	if (val.isNull()) {
		csv_trim = false;
		parentApp->toolkit_settings.setValue("csv.trim", csv_trim);
	} else {
		csv_trim = val.toBool();
	}

	parentApp->toolkit_settings.endGroup();
}

void AnimationPlugin::action_load_animation() {
	if (parentApp != NULL) {
		QFileDialog file_dialog (parentApp, "Select Animation File");

		file_dialog.setNameFilter(tr("Animation File (*.csv *.txt)"));
		file_dialog.setFileMode(QFileDialog::ExistingFile);

		if (file_dialog.exec()) {
			AnimationModelExtention* ext;
			try {
				ext = loadAnimationFile (file_dialog.selectedFiles().at(0));
			} catch (RigidBodyDynamics::Errors::RBDLError& e) {
				ToolkitApp::showExceptionDialog(e);
				return;
			}

			if (parentApp->getLoadedModels()->size() != 0) {
				RBDLModelWrapper* rbdl_model = nullptr;

				if (parentApp->getLoadedModels()->size() == 1) {
					rbdl_model = parentApp->getLoadedModels()->at(0);
				} else {
					rbdl_model = parentApp->selectModel(nullptr);
				}

				if (rbdl_model != nullptr) {
					rbdl_model->addExtention(ext);
					parentApp->getToolkitTimeline()->setMaxTime(ext->getMaxTime());
					loaded_animations.push_back(ext);
				} else {
					delete ext;
				}
			}
		}	


	} else {
		throw RigidBodyDynamics::Errors::RBDLError("Animation plugin was not initialized correctly!");
	}
}

void AnimationPlugin::reload_files() {

}

AnimationModelExtention* AnimationPlugin::loadAnimationFile(QString path) {
	AnimationModelExtention* animation = new AnimationModelExtention();

	std::vector<std::string> names;
	std::vector<float> row_values;

	//make sure local is en_US for consistent float parsing
	std::setlocale(LC_NUMERIC, "en_US.UTF-8");

	io::filtering_istream csv_stream;

	std::ifstream file(path.toStdString().c_str(), std::ios_base::in);
	CSV_IOstream_Sanatiser<char> csv_sanatiser;
	csv_stream.push(csv_sanatiser);
	csv_stream.push(file);

	aria::csv::CsvParser parser(csv_stream);
	parser.delimiter(csv_seperator);

	bool has_header = false;
	bool first_row_parsed = false;
	unsigned int first_row_count = 0;

	unsigned int header_dof = 0;
	unsigned int animation_dof = 0;

	QString first_entry;
	bool ok = true;

	for (auto& row : parser) {
		for (auto& field : row) {
			if (!first_row_parsed) {
				if (boost::algorithm::contains(field, "DATA_FROM:")) {
					throw RBDLToolkitError("Error: RBDL-Toolkit does not support external data files. Please put your animation data direktly below the DATA: tag!\n");
				}
				first_row_count++;
				if (ok) {
					first_entry = QString::fromStdString(field);
					first_entry.toFloat(&ok);
					if (!ok) {
						has_header = true;
					}
				}
			}
			if (has_header && !first_row_parsed) {
				names.push_back(field);
			} else {
				row_values.push_back(QString::fromStdString(field).toFloat());
			}
		}

		if (!has_header || first_row_parsed) {
			if (animation_dof == 0 && row_values.size() != 0) {
				animation_dof = row_values.size() - 1;
			} else if (row_values.size() == 0) {
				continue;
			} else if (animation_dof != row_values.size()-1) {
				throw RBDLToolkitError("Inconsistent amount of columns in animation file! Can not display Animation!\n");
			}
			float time = row_values[0];
			RigidBodyDynamics::Math::VectorNd data(animation_dof);
			//std::cout << time << " [ "; 
			for ( int j = 0; j < animation_dof; j++) {
				data[j] = row_values[j+1];
				//std::cout << data[j] << ", ";
			}
			//std::cout << "]" << std::endl; 

			//write read values to animation object
			animation->addAnimationFrame(time, data);
		}

		//this only gets executed once after the first row was read
		if (!first_row_parsed) {
			first_row_parsed = true;
		}

		//reset array for next row
		row_values.clear();
	}

	if (has_header) {
		parentApp->showWarningDialog("Detected Header in CSV: Warning RBDL Toolkit does not assume the order of values in the header to be correct. It expects the values to be ordered accoring to the order in the RBDL Model. Please make sure this is the case!\n");
	}

	return animation;
}
