#include "SceneWidget.h"

#include <Qt3DCore/QTransform>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QCameraLens>

#include <Qt3DInput/QInputAspect>

#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QOrbitCameraController>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;
using namespace Qt3DLogic;

SceneWidget::SceneWidget(QWidget *parent): QWidget(parent), fov(45.f), near(0.1f), far(100.f) {
	qt3d_view = new Qt3DExtras::Qt3DWindow();
    qt3d_view->defaultFrameGraph()->setClearColor(QColor("black"));

	scene_root = new Qt3DCore::QEntity();

	frame_action = new QFrameAction(scene_root);
	connect(frame_action, SIGNAL(triggered(float)), this, SLOT(frame_action_repeater(float)));

	world_lighting = new Qt3DRender::QDirectionalLight(scene_root);
	world_lighting->setWorldDirection(QVector3D(1., 1., 1.));

    camera = qt3d_view->camera();
	camera->lens()->setPerspectiveProjection(fov, 16.0f/9.0f, near, far);
	camera->setPosition(QVector3D(6., 3., 6.0f));
	camera->setUpVector(QVector3D(0, 1, 0));
	camera->setViewCenter(QVector3D(0, 0, 0));

	auto *camera_controller = new QOrbitCameraController(scene_root); 
	camera_controller->setCamera(camera);

	qt3d_view->setRootEntity(scene_root);

	display_container = new QHBoxLayout(this);
	display_container->setMargin(0);
	display_container->addWidget(QWidget::createWindowContainer(qt3d_view));

	render_selector = new QWidget();
	render_selector->setMinimumWidth(150);

	render_selector_layout = new QVBoxLayout;
	render_selector_layout->setDirection(QBoxLayout::TopToBottom);
	render_selector_layout->setAlignment(Qt::AlignTop);
	render_selector_layout->setSpacing(0);
	render_selector_layout->setMargin(0);

	render_selector->setLayout(render_selector_layout);

	main_app = qobject_cast<ToolkitApp*>(parent);
	main_app->addView("RenderSelect", render_selector);
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

		bool check_box_exsist = false;
		for (RenderGroupCheckBox* box : render_select_check_boxes) {
			if (box->text() == group_name) {
				check_box_exsist = true;
				break;
			}
		}

		if (!check_box_exsist) {
			RenderGroupCheckBox* check_box = new RenderGroupCheckBox(group_name);
			check_box->setCheckState(Qt::Checked);

			connect(check_box, SIGNAL(renderGroupStateChanged(QString, bool)), this, SLOT(toggle_render_obj_group(QString, bool)));

			render_select_check_boxes.push_back(check_box);
			render_selector_layout->addWidget(check_box, 0, Qt::AlignTop);
		}
	}
}

void SceneWidget::removeSceneObject(Qt3DCore::QEntity *scene_obj) {
	scene_obj->setParent((Qt3DCore::QEntity*) nullptr);

	QVariant obj_grouping = scene_obj->property("Scene.ObjGroup");
	if (obj_grouping.isValid()) {
		QString group_name = obj_grouping.toString();
		for(auto it=scene_obj_grouping[group_name].begin(); it<scene_obj_grouping[group_name].end(); it++) {
			if (*it == scene_obj) {
    			scene_obj_grouping[group_name].erase(it);
				break;
			}
		}
	}
}

void SceneWidget::toggle_render_obj_group(QString group_name, bool status) {
	auto it = scene_obj_grouping.find(group_name);

	//if the group exsits set visibility of the entities 
	if ( it != scene_obj_grouping.end()) {
		std::vector<Qt3DCore::QEntity*> entities = it->second;
		for ( QEntity* ntty : entities ) {
			ntty->setEnabled(status);
		}
	}
}

void SceneWidget::frame_action_repeater(float dt) {
	emit frame_sync_signal(dt);
}
