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
}

void MotionMarkerPlugin::action_load_data() {
}

void MotionMarkerPlugin::reload_files() {

}

void MotionMarkerPlugin::loadMotionMarkerFile(QString path) {
}
