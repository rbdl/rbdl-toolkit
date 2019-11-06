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
}

CameraOperatorPlugin::~CameraOperatorPlugin() {
}

void CameraOperatorPlugin::init(ToolkitApp* app) {
	//save reference to parent ToolkitApp 
	parentApp = app;

	camera_menu.setTitle("Camera");

	QAction* front_view = camera_menu.addAction("Front View");
	front_view->setShortcut(QKeySequence::fromString("1"));
	connect(front_view, &QAction::triggered, [=]
	       	{
				setFrontView();
	       	});

	QAction* side_view = camera_menu.addAction("Side View");
	side_view->setShortcut(QKeySequence::fromString("2"));
	connect(side_view, &QAction::triggered, [=]
	       	{
				setSideView();
	       	});

	QAction* top_view = camera_menu.addAction("Top View");
	top_view->setShortcut(QKeySequence::fromString("3"));
	connect(top_view, &QAction::triggered, [=]
	       	{
				setTopView();
	       	});

	QAction* orthographic_toggle = camera_menu.addAction("Orthograpic View");
	orthographic_toggle->setCheckable(true);
	orthographic_toggle->setShortcut(QKeySequence::fromString("5"));
	connect(orthographic_toggle, &QAction::changed, [=]
	       	{
		       	setOrthographicView(orthographic_toggle->isChecked());
	       	});

	parentApp->addMenu(&camera_menu);

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
	camera->setPosition(QVector3D(0, 5, 0));
	camera->setViewCenter(QVector3D(0, 0, 0));
}

void CameraOperatorPlugin::setSideView() {
	Qt3DRender::QCamera* camera = parentApp->getSceneObj()->getCameraObj();
	camera->setPosition(QVector3D(0, 0, 5));
	camera->setViewCenter(QVector3D(0, 0, 0));
}

void CameraOperatorPlugin::setFrontView() {
	Qt3DRender::QCamera* camera = parentApp->getSceneObj()->getCameraObj();
	camera->setPosition(QVector3D(5, 0, 0));
	camera->setViewCenter(QVector3D(0, 0, 0));
}
