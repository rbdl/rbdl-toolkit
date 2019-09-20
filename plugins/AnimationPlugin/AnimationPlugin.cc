#include "AnimationPlugin.h"
#include <iostream>

#include <QFileDialog>

using namespace RigidBodyDynamics::Math;

AnimationPlugin::AnimationPlugin() {
	parentApp = NULL;
}

AnimationPlugin::~AnimationPlugin() {
}

void AnimationPlugin::init(ToolkitApp* app) {
	//save reference to parent ToolkitApp 
	parentApp = app;

	load_file_trigger = new QAction("Load Animation File");
	parentApp->addFileAction(load_file_trigger);

	connect(load_file_trigger, SIGNAL(triggered(bool)), this, SLOT(action_load_animation()));

	//std::cout << "Core Plugin for Animations loaded!" << std::endl;
}

void AnimationPlugin::action_load_animation() {
	//QFileDialog file_dialog (parentApp, "Select Animation File");

	//file_dialog.setNameFilter(tr("Animation File (*csv, *.txt)"));
	//file_dialog.setFileMode(QFileDialog::ExistingFile);

	//if (file_dialog.exec()) {
	//	loadAnimationFile (file_dialog.selectedFiles().at(0));
	//}	

	if (parentApp != NULL) {
		if (parentApp->getLoadedModels()->size() != 0) {
			RBDLModelWrapper* rbdl_model = (*parentApp->getLoadedModels())[0];

			VectorNd start = VectorNd::Zero(rbdl_model->getModelDof());
			VectorNd end = VectorNd::Zero(rbdl_model->getModelDof());
			end[0] = -5.;
			end[1] = -5.;
			end[2] = -5.;

			AnimationModelExtention* ext = new AnimationModelExtention();
			ext->addAnimationFrame(0., start);
			ext->addAnimationFrame(10., end);

			rbdl_model->addExtention(ext);
			parentApp->getToolkitTimeline()->setMaxTime(10.);
		}
	}
}

void AnimationPlugin::reload_files() {

}

AnimationModelExtention* AnimationPlugin::loadAnimationFile(QString path) {
	AnimationModelExtention* animation = new AnimationModelExtention();

	return animation;
}

