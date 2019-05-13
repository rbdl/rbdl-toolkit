#include "SceneWidget.h"

#include <Qt3DCore/QTransform>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QCameraLens>

#include <Qt3DInput/QInputAspect>

#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QOrbitCameraController>

#include "ToolkitApp.h"

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

SceneWidget::SceneWidget(QWidget *parent): QWidget(parent), fov(45.f), near(0.1f), far(100.f) {
	qt3d_view = new Qt3DExtras::Qt3DWindow();
    qt3d_view->defaultFrameGraph()->setClearColor(QColor("black"));

	scene_root = new Qt3DCore::QEntity();

	world_lighting = new Qt3DRender::QDirectionalLight(scene_root);
	world_lighting->setWorldDirection(QVector3D(1., 1., 1.));

    camera = qt3d_view->camera();
	camera->lens()->setPerspectiveProjection(fov, 16.0f/9.0f, near, far);
	camera->setPosition(QVector3D(5., 5., 5.0f));
	camera->setUpVector(QVector3D(0, 1, 0));
	camera->setViewCenter(QVector3D(0, 0, 0));

	auto *camera_controller = new QOrbitCameraController(scene_root); 
	camera_controller->setCamera(camera);

	qt3d_view->setRootEntity(scene_root);

	display_container = new QHBoxLayout(this);
	display_container->setMargin(0);
	display_container->addWidget(QWidget::createWindowContainer(qt3d_view));

	ToolkitApp* app = qobject_cast<ToolkitApp*>(parent);
	app->addView("Test", new QWidget);
}

void SceneWidget::resizeEvent(QResizeEvent *event) {
	//qDebug() << event->size().width() << ", " << event->size().height() ;
	width = event->size().width();
	height = event->size().height();

	camera->lens()->setPerspectiveProjection(fov, width/height, near, far);
}

void SceneWidget::addSceneObject(Qt3DCore::QEntity *scene_obj) {
	//qDebug() << scene_obj->dynamicPropertyNames();
	scene_obj->setParent(scene_root);

	//add scene_obj to map of obj_groups for easiely toggeling if the obj is rendered
	QVariant obj_grouping = scene_obj->property("Scene.ObjGroup");
	if (obj_grouping.isValid()) {
		QString group_name = obj_grouping.toString();
		scene_obj_grouping[group_name].push_back(scene_obj);
	}
}
