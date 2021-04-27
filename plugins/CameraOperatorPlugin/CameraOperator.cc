#include "CameraOperator.h"
#include <iostream>
#include <clocale>

#include <QFileDialog>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <Qt3DRender>

using namespace RigidBodyDynamics::Math;

CameraOperatorPlugin::CameraOperatorPlugin() {
	parentApp = NULL;
	default_distance = 5.0;
	camera_reset_enabled = true;
}

CameraOperatorPlugin::~CameraOperatorPlugin() {
	parentApp->deleteMenu(camera_menu);
}

void CameraOperatorPlugin::init(ToolkitApp* app) {
	//save reference to parent ToolkitApp 
	parentApp = app;

	camera_menu = app->getMenu("Camera");

	// Camera direct actions
	QAction* front_view = camera_menu->addAction("Front View");
	front_view->setShortcut(QKeySequence::fromString("1"));
	connect(front_view, &QAction::triggered, this, &CameraOperatorPlugin::setFrontView);

	QAction* side_view = camera_menu->addAction("Side View");
	side_view->setShortcut(QKeySequence::fromString("2"));
	connect(side_view, &QAction::triggered, this, &CameraOperatorPlugin::setSideView);

	QAction* top_view = camera_menu->addAction("Top View");
	top_view->setShortcut(QKeySequence::fromString("3"));
	connect(top_view, &QAction::triggered, this, &CameraOperatorPlugin::setTopView);

	QAction* orthographic_toggle = camera_menu->addAction("Orthograpic View");
	orthographic_toggle->setCheckable(true);
	orthographic_toggle->setShortcut(QKeySequence::fromString("5"));
	connect(orthographic_toggle, &QAction::changed, [=]
	       	{
		       	setOrthographicView(orthographic_toggle->isChecked());
	       	});

	QAction* reset_camera = camera_menu->addAction("Reset Camera");
	connect(reset_camera, &QAction::triggered, this, &CameraOperatorPlugin::resetCamera);

	QAction* save_camera = camera_menu->addAction("Save Camera");
	connect(save_camera, &QAction::triggered, this, &CameraOperatorPlugin::saveCamera);

	loadCameraSettings();

	//connect settings reload
	connect(&parentApp->toolkit_settings, &ToolkitSettings::settings_changed, this, &CameraOperatorPlugin::reloadSettings);
	//connet restet camrea
	connect(parentApp, &ToolkitApp::reload, this, &CameraOperatorPlugin::reset);

	Qt3DRender::QCamera* camera = parentApp->getSceneObj()->getCameraObj();
	camera->setPosition(camera_pos);
	camera->setViewCenter(camera_poi);
}

void CameraOperatorPlugin::loadCameraSettings() {
	parentApp->toolkit_settings.beginGroup("CameraOptions");

	// default distance
	QVariant val = parentApp->toolkit_settings.value("Camera.default_distance");
	if (val.isNull()) {
		parentApp->toolkit_settings.setValue("Camera.default_distance", default_distance);
	} else {
		default_distance = val.toDouble();
	}
	parentApp->toolkit_settings.setType("Camera.default_distance", default_distance);

	// camera reset enabled
	val = parentApp->toolkit_settings.value("Camera.reset_enabled");
	if (val.isNull()) {
		parentApp->toolkit_settings.setValue("Camera.reset_enabled", camera_reset_enabled);
	} else {
		camera_reset_enabled = val.toBool();
	}
	parentApp->toolkit_settings.setType("Camera.reset_enabled", camera_reset_enabled);

	// camera positon
	val = parentApp->toolkit_settings.value("Camera.position");
	if (val.isNull()) {
		camera_pos = parentApp->getSceneObj()->getCameraObj()->position();
		parentApp->toolkit_settings.setValue("Camera.position", camera_pos);
	} else {
		camera_pos = val.value<QVector3D>();
	}
	parentApp->toolkit_settings.setType("Camera.position", camera_pos);

	// camera view center
	val = parentApp->toolkit_settings.value("Camera.view_center");
	if (val.isNull()) {
		camera_poi = parentApp->getSceneObj()->getCameraObj()->viewCenter();
		parentApp->toolkit_settings.setValue("Camera.view_center", camera_poi);
	} else {
		camera_poi = val.value<QVector3D>();
	}
	parentApp->toolkit_settings.setType("Camera.view_center", camera_poi);


	parentApp->toolkit_settings.endGroup();
}

void CameraOperatorPlugin::setOrthographicView(bool state) {
	if (state) {
		parentApp->getSceneObj()->setCameraLens(Qt3DRender::QCameraLens::OrthographicProjection);
	} else {
		parentApp->getSceneObj()->setCameraLens(Qt3DRender::QCameraLens::PerspectiveProjection);
	}
}

void CameraOperatorPlugin::setTopView() {
	Qt3DRender::QCamera* camera = parentApp->getSceneObj()->getCameraObj();
	camera->setPosition(QVector3D(0, default_distance, 0));
	camera->setViewCenter(QVector3D(0, 0, 0));
	camera->setUpVector(QVector3D(0, 1, 0));
}

void CameraOperatorPlugin::setSideView() {
	Qt3DRender::QCamera* camera = parentApp->getSceneObj()->getCameraObj();
	camera->setPosition(QVector3D(default_distance, 0, 0));
	camera->setViewCenter(QVector3D(0, 0, 0));
	camera->setUpVector(QVector3D(0, 1, 0));
}

void CameraOperatorPlugin::setFrontView() {
	Qt3DRender::QCamera* camera = parentApp->getSceneObj()->getCameraObj();
	camera->setPosition(QVector3D(0, 0, default_distance));
	camera->setViewCenter(QVector3D(0, 0, 0));
	camera->setUpVector(QVector3D(0, 1, 0));
}

void CameraOperatorPlugin::saveCamera() {
	Qt3DRender::QCamera* camera = parentApp->getSceneObj()->getCameraObj();
	camera_pos = camera->position();
	camera_poi = camera->viewCenter();

	parentApp->toolkit_settings.beginGroup("CameraOptions");
	parentApp->toolkit_settings.setValue("Camera.position", camera_pos);
	parentApp->toolkit_settings.setValue("Camera.view_center", camera_poi);
	parentApp->toolkit_settings.endGroup();
}

void CameraOperatorPlugin::resetCamera() {
	Qt3DRender::QCamera* camera = parentApp->getSceneObj()->getCameraObj();
	camera->setPosition(camera_pos);
	camera->setViewCenter(camera_poi);
	camera->setUpVector(QVector3D(0, 1, 0));
}

void CameraOperatorPlugin::reset() {
	if (camera_reset_enabled) {
		resetCamera();
	}
}

void CameraOperatorPlugin::reloadSettings() {
	loadCameraSettings();
}
