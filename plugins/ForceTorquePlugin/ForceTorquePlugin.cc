#include "ForceTorquePlugin.h"
#include <iostream>
#include <clocale>

#include <QFileDialog>
#include <QMessageBox>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QTransform>

#include <parser.hpp>
#include <toolkit_errors.h>

using namespace RigidBodyDynamics::Math;

ForceTorquePlugin::ForceTorquePlugin() {
	parentApp = NULL;
}

ForceTorquePlugin::~ForceTorquePlugin() {
}

void ForceTorquePlugin::init(ToolkitApp* app) {
	//save reference to parent ToolkitApp 
	parentApp = app;

	load_file_trigger = new QAction("Load Forces/Torques");
	parentApp->addFileAction(load_file_trigger);

	connect(load_file_trigger, SIGNAL(triggered(bool)), this, SLOT(action_load_data()));

	getCSVSettings();
	getArrowSettings();

	QString force_arrow_src = findFile(QString("force_arrow.obj"));
	force_arrow_mesh = new Qt3DRender::QMesh;
	force_arrow_mesh->setSource(QUrl::fromLocalFile(force_arrow_src));

	QString torque_arrow_src = findFile(QString("torque_arrow.obj"));
	torque_arrow_mesh = new Qt3DRender::QMesh;
	torque_arrow_mesh->setSource(QUrl::fromLocalFile(torque_arrow_src));

	QCommandLineOption force_torque_option( QStringList() << "ft" << "forcetorque",
	                                 "Load Force/Torque files <file>", 
	                                 "file"
	                               );
	parentApp->addCmdOption(force_torque_option, [this](QCommandLineParser& parser){
		auto data_list = parser.values("forcetorque");

		for (int i=0; i<data_list.size(); i++) {
			if (i < parentApp->getLoadedModels()->size() ) {
				ArrowFieldModelExtention *force_field = new ArrowFieldModelExtention(force_arrow_mesh, "Forces", force_color, draw_threshold, arrow_scale_factor);
				ArrowFieldModelExtention *torque_field = new ArrowFieldModelExtention(torque_arrow_mesh, "Torques", torque_color, draw_threshold, arrow_scale_factor);

				try {
					this->loadForceTorqueFile(data_list[i], force_field, torque_field);
				} catch (RigidBodyDynamics::Errors::RBDLError& e){
					ToolkitApp::showExceptionDialog(e);
					delete force_field;
					delete torque_field;
					continue;
				}
				RBDLModelWrapper* model = parentApp->getLoadedModels()->at(i);
				model->addExtention(force_field);
				model->addExtention(torque_field);
			} else {
				std::cout << QString("Force/Torque file %1 can not be mapped to a model ... Ignoring!").arg(data_list[i]).toStdString() << std::endl;
			}
		}
	});
}
void ForceTorquePlugin::getArrowSettings() {
	parentApp->toolkit_settings.beginGroup("RenderOptions");


	//force color setting
	QVariant val = parentApp->toolkit_settings.value("force.color");
	if (val.isNull()) {
		force_color = QColor::fromRgbF(1., 0., 0., 0.9);
		parentApp->toolkit_settings.setValue("force.color", force_color.rgba());
	} else {
		//read as int because it is saved as one, otherwise it would not load correctly
		//watch out for overflow -> will silently fail because char is uint8
		force_color = QColor::fromRgba(val.toUInt());
	}

	//torque color setting
	val = parentApp->toolkit_settings.value("torque.color");
	if (val.isNull()) {
		torque_color = QColor::fromRgbF(0., 1., 0., 0.9);
		parentApp->toolkit_settings.setValue("torque.color", torque_color.rgba());
	} else {
		//read as int because it is saved as one, otherwise it would not load correctly
		//watch out for overflow -> will silently fail because char is uint8
		torque_color = QColor::fromRgba(val.toUInt());
	}

	//arrow display size threshold
	val = parentApp->toolkit_settings.value("arrow.display_threshold");
	if (val.isNull()) {
		draw_threshold = 0.001;
		parentApp->toolkit_settings.setValue("arrow.display_threshold", draw_threshold);
	} else {
		draw_threshold = val.toFloat();
	}

	val = parentApp->toolkit_settings.value("arrow.arrow_scale_factor");
	if (val.isNull()) {
		arrow_scale_factor = 0.001;
		parentApp->toolkit_settings.setValue("arrow.arrow_scale_factor", arrow_scale_factor);
	} else {
		arrow_scale_factor = val.toFloat();
	}

	parentApp->toolkit_settings.endGroup();
}

void ForceTorquePlugin::getCSVSettings() {
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

void ForceTorquePlugin::action_load_data() {
	ArrowFieldModelExtention *field = new ArrowFieldModelExtention(force_arrow_mesh, "Forces", force_color);

	if (parentApp != NULL) {
		QFileDialog file_dialog (parentApp, "Select Force/Torque Data File");

		file_dialog.setNameFilter(tr("Force/Torque File (*.csv *.ff)"));
		file_dialog.setFileMode(QFileDialog::ExistingFile);

		ArrowFieldModelExtention *force_field = new ArrowFieldModelExtention(force_arrow_mesh, "Forces", force_color, draw_threshold, arrow_scale_factor);
		ArrowFieldModelExtention *torque_field = new ArrowFieldModelExtention(torque_arrow_mesh, "Torques", torque_color, draw_threshold, arrow_scale_factor);

		if (file_dialog.exec()) {
			try {
				loadForceTorqueFile(file_dialog.selectedFiles().at(0), force_field, torque_field);
			} catch (RigidBodyDynamics::Errors::RBDLError& e){
				ToolkitApp::showExceptionDialog(e);
				delete force_field;
				delete torque_field;
			}
			if (parentApp->getLoadedModels()->size() != 0) {
				RBDLModelWrapper* rbdl_model = nullptr;

				if (parentApp->getLoadedModels()->size() == 1) {
					rbdl_model = parentApp->getLoadedModels()->at(0);
				} else {
					rbdl_model = parentApp->selectModel(nullptr);
				}

				if (rbdl_model != nullptr) {
					rbdl_model->addExtention(force_field);
					rbdl_model->addExtention(torque_field);
				} else {
					delete force_field;
					delete torque_field;
				}
			}
		}	
	} else {
		//should never happen
		throw RigidBodyDynamics::Errors::RBDLError("Force/Torque plugin was not initialized correctly!");
	}
}

void ForceTorquePlugin::reload_files() {

}

void ForceTorquePlugin::loadForceTorqueFile(QString path, ArrowFieldModelExtention* force_field, ArrowFieldModelExtention *torque_field) {
	std::setlocale(LC_NUMERIC, "en_US.UTF-8");

	std::vector<std::string> names;
	std::vector<float> row_values;

	std::ifstream file(path.toStdString().c_str(), std::ios_base::in);
	aria::csv::CsvParser parser(file);
	parser.delimiter(csv_seperator);

	bool header_check = false;

	for (auto& row : parser) {
		bool ok = true;
		if (!header_check) {
			header_check = true; 

			QString first_entry = QString::fromStdString(row[0]);
			first_entry.toFloat(&ok);
			if (!ok) {
				//found header
				for (auto& field : row) {
					names.push_back(field);
				}
			}
		}
		if (!ok) continue;

		//check for correct column size
		//column format : time + ( pos_x, pos_y, pos_z, force_x, force_y, force_z, torque_x, torque_y, torque_z)+
		if ( (row.size() - 1) % 9 != 0 ) {
			throw RBDLToolkitError("Could not load Force / Torque data from file, due to incorrect amount of data columns!\n");
		}
		int entry_count = (row.size() - 1) / 9;

		for (auto& field : row) {
			row_values.push_back(QString::fromStdString(field).toFloat());
		}

		float time = row_values[0];
		Matrix3fd pos, force_data, torque_data;
		pos.resize(3, entry_count);
		force_data.resize(3, entry_count);
		torque_data.resize(3, entry_count);

		for (int i=0; i<entry_count; i++) {
			pos(0, i) = row_values[ (9*i+0) +1 ];
			pos(1, i) = row_values[ (9*i+1) +1 ];
			pos(2, i) = row_values[ (9*i+2) +1 ];

			force_data(0, i) = row_values[ (9*i+3) +1 ];
			force_data(1, i) = row_values[ (9*i+4) +1 ];
			force_data(2, i) = row_values[ (9*i+5) +1 ];

			torque_data(0, i) = row_values[ (9*i+6) +1 ];
			torque_data(1, i) = row_values[ (9*i+7) +1 ];
			torque_data(2, i) = row_values[ (9*i+8) +1 ];
 		}

 		force_field->addArrowFieldFrame(time, pos, force_data);
 		torque_field->addArrowFieldFrame(time, pos, torque_data);

 		row_values.clear();
	}
}
