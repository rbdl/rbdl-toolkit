#include "SceneWidget.h"

SceneWidget::SceneWidget(QWidget *parent): QWidget(parent) {
	qt3d_view = new Qt3DExtras::Qt3DWindow();
    qt3d_view->defaultFrameGraph()->setClearColor(QColor("black"));



	scene_root = new Qt3DCore::QEntity();
    camera = qt3d_view->camera();

	camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
	camera->setPosition(QVector3D(0, 0, 20.0f));
	camera->setUpVector(QVector3D(0, 1, 0));
	camera->setViewCenter(QVector3D(0, 0, 0));

	qt3d_view->setRootEntity(scene_root);

	display_container = new QHBoxLayout(this);
	display_container->setMargin(0);
	display_container->addWidget(QWidget::createWindowContainer(qt3d_view));
}
