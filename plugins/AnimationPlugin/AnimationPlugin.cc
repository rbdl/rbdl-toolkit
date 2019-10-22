#include "AnimationPlugin.h"
#include <iostream>
#include <clocale>

#include <QFileDialog>

#include <rapidcsv.h>

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

		file_dialog.setNameFilter(tr("Animation File (*csv, *.txt)"));
		file_dialog.setFileMode(QFileDialog::ExistingFile);

		if (file_dialog.exec()) {
			AnimationModelExtention* ext = loadAnimationFile (file_dialog.selectedFiles().at(0));

			if (parentApp->getLoadedModels()->size() != 0) {
				RBDLModelWrapper* rbdl_model = (*parentApp->getLoadedModels())[0];

				rbdl_model->addExtention(ext);
				parentApp->getToolkitTimeline()->setMaxTime(ext->getMaxTime());
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

	//make sure local is en_US for consistent float parsing
	std::setlocale(LC_NUMERIC, "en_US.UTF-8");

	rapidcsv::Document animation_file(
		path.toStdString(), 
		rapidcsv::LabelParams(-1, 0),
		rapidcsv::SeparatorParams(csv_seperator, csv_trim));

	int animation_dof = animation_file.GetColumnCount();
	QString first_entry = QString::fromStdString(animation_file.GetCell<std::string>(-1, 0));
	bool header_found;

	// if the first entry in csv is not a string, then it does not have a header
	// if it has a header we need to read it first, otherwise just read in the
	// values
	//auto names = animation_file.GetColumnNames();

	bool ok;
	first_entry.toFloat(&ok);

	int start = 0;
	if (ok) {
		//no header found
		header_found = false;
		start = 0;
	} else {
		//header found
		header_found = true;
		start = 1;
	}

	for (int i=start; i<animation_file.GetRowCount()-1; i++) {
		auto values = animation_file.GetRow<float>(i);
		float time = animation_file.GetCell<float>(-1, i);

		RigidBodyDynamics::Math::VectorNd data(animation_dof);

		for ( int j = 0; j < animation_dof; j++) {
			data[j] = values[j];
		}

		animation->addAnimationFrame(time, data);
	}

	return animation;
}

