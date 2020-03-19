#include "MotionMarkerPlugin.h"
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
#include <rbdl_wrapper.h>

using namespace RigidBodyDynamics::Math;

MotionMarkerPlugin::MotionMarkerPlugin() {
	parentApp = NULL;
}

MotionMarkerPlugin::~MotionMarkerPlugin() {
}

void MotionMarkerPlugin::init(ToolkitApp* app) {
	//save reference to parent ToolkitApp 
	parentApp = app;

	load_file_trigger = new QAction("Load Motion Markers");
	parentApp->addFileAction(load_file_trigger);

	connect(load_file_trigger, SIGNAL(triggered(bool)), this, SLOT(action_load_data()));

	QCommandLineOption motion_marker_option( QStringList() << "mm" << "motionmarker",
	                                 "Load Motion Marker files <file>", 
	                                 "file"
	                               );
	parentApp->addCmdOption(motion_marker_option, [this](QCommandLineParser& parser){
		auto data_list = parser.values("motionmarker");
	});

	//display model markers of all loaded models
	auto all_models = parentApp->getLoadedModels();
	for (auto model : *all_models) {
		unsigned int segment_cnt = model->model_luatable["frames"].length();

		for(int i=1; i<=segment_cnt; i++) {
			std::string segment_name = model->model_luatable["frames"][i]["name"].get<std::string>();
			std::vector<LuaKey> keys= model->model_luatable["frames"][i]["markers"].keys();

			std::cout << segment_name << " marker_count: " << keys.size() << std::endl; 
		}
	}
}

void MotionMarkerPlugin::action_load_data() {
}

void MotionMarkerPlugin::reload_files() {

}

void MotionMarkerPlugin::loadMotionMarkerFile(QString path) {
}
