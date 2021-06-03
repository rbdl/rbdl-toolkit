#include "SceneWidget.h"

#include <Qt3DCore/QTransform>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QCameraLens>

#include <Qt3DInput/QInputAspect>

#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <QWindow>
#include <QSurface>
#include <QSurfaceFormat>

#include <math.h>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;
using namespace Qt3DLogic;

SceneWidget::SceneWidget(QWidget *parent): 
	QWidget(parent),
	fov(45.f),
	near_v(0.1f),
	far_v(100.f),
	render_capture(nullptr),
	//use almost black, because just black is transparent at application start up 🙈
	default_clear_color(QColor::fromRgbF(0.01, 0.01, 0.01, 1.)) 
{
	//set default surface format to be able to use the alpha channel from the start
	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
#ifdef QT_OPENGL_ES_2
		format.setRenderableType(QSurfaceFormat::OpenGLES);
#else
		if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
			format.setVersion(4, 3);
			format.setProfile(QSurfaceFormat::CoreProfile);
		}
#endif
	format.setAlphaBufferSize(8);
	format.setSamples(16);
	QSurfaceFormat::setDefaultFormat(format);

	//setup actual widget
	qt3d_view = new Qt3DExtras::Qt3DWindow();
    qt3d_view->defaultFrameGraph()->setClearColor(default_clear_color);

	scene_root = new Qt3DCore::QEntity();

	frame_action = new QFrameAction(scene_root);
	connect(frame_action, SIGNAL(triggered(float)), this, SLOT(frame_action_repeater(float)));

	world_lighting = new Qt3DRender::QDirectionalLight(scene_root);
	world_lighting->setWorldDirection(QVector3D(1., 1., 1.));

    camera = qt3d_view->camera();
	camera->lens()->setPerspectiveProjection(fov, 16.0f/9.0f, near_v, far_v);
	camera->setPosition(QVector3D(6., 3., 6.0f));
	camera->setUpVector(QVector3D(0, 1, 0));
	camera->setViewCenter(QVector3D(0, 0, 0));

	auto *camera_controller = new QOrbitCameraController(scene_root); 
	camera_controller->setCamera(camera);
	camera_controller->setZoomInLimit(0.05);

	qt3d_view->setRootEntity(scene_root);

	display_container = new QHBoxLayout(this);
	display_container->setContentsMargins(QMargins(0,0,0,0));
	display_container->addWidget(QWidget::createWindowContainer(qt3d_view));

	render_selector = new SceneGroupsWidget(this);

	main_app = qobject_cast<ToolkitApp*>(parent);
	main_app->addView("RenderSelect", qobject_cast<QWidget*>(render_selector));
}

void SceneWidget::resizeEvent(QResizeEvent *event) {
	width = event->size().width();
	height = event->size().height();

	setCameraLens(camera->lens()->projectionType());
}

void SceneWidget::setCameraLens(QCameraLens::ProjectionType projection) {
	//std::cout << width << " " << height << std::endl;
	camera->lens()->setProjectionType(projection);
	if (projection == QCameraLens::PerspectiveProjection) {
		camera->lens()->setPerspectiveProjection( fov,
		                                          float(width)/float(height), 
		                                          near_v, far_v);
	} else if (projection == QCameraLens::OrthographicProjection) {
		QVector3D eye = camera->viewVector();
		QVector3D poi = camera->viewCenter();

		float aspect = float(width)/float(height);
		float dist = eye.distanceToPoint(poi);

		float w = tan(fov * M_PI / 180.) * 0.001 * width * dist / 2.;
		float h = w / aspect;

		camera->lens()->setOrthographicProjection( -0.5 * w,
		                                            0.5 * w,
		                                           -0.5 * h,
		                                            0.5 * h,
		                                            near_v, far_v);
	}
}

void SceneWidget::addSceneObject(Qt3DCore::QEntity *scene_obj) {
	//qDebug() << scene_obj->dynamicPropertyNames();
	scene_obj->setParent(scene_root);

	//add scene_obj to map of obj_groups for easiely toggeling if the obj is rendered
	QVariant obj_grouping = scene_obj->property("Scene.ObjGroup");
	if (obj_grouping.isValid()) {
		QString group_name = obj_grouping.toString();
		addSceneObjectToGroup(scene_obj, group_name);
	}

	int i=0;
	for ( auto child : scene_obj->findChildren<QEntity*>()) {
		QVariant obj_grouping = child->property("Scene.ObjGroup");
		if (obj_grouping.isValid()) {
			QString group_name = obj_grouping.toString();
			addSceneObjectToGroup(qobject_cast<QEntity*>(child), group_name);
		}
	}
}

void SceneWidget::addSceneObjectToGroup(QEntity* scene_obj, QString group_name) {
	bool found = false;
	for(auto it=scene_obj_grouping[group_name].begin(); it<scene_obj_grouping[group_name].end(); it++) {
		if (*it == scene_obj) {
			found = true;
			break;
		}
	}
	if (!found)
		scene_obj_grouping[group_name].push_back(scene_obj);
	else
		return;

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
		render_selector->addSelector(check_box);
	}
}

void SceneWidget::removeSceneObjectFromGroup(QEntity* scene_obj, QString group_name) {
	for(auto it=scene_obj_grouping[group_name].begin(); it<scene_obj_grouping[group_name].end(); it++) {
		if (*it == scene_obj) {
			scene_obj_grouping[group_name].erase(it);
			break;
		}
	}
}

void SceneWidget::removeSceneObject(Qt3DCore::QEntity *scene_obj) {
	scene_obj->setParent((Qt3DCore::QEntity*) nullptr);

	QVariant obj_grouping = scene_obj->property("Scene.ObjGroup");
	if (obj_grouping.isValid()) {
		QString group_name = obj_grouping.toString();
		removeSceneObjectFromGroup(scene_obj, group_name);
	}
	for ( auto child : scene_obj->findChildren<QEntity*>()) {
		QVariant obj_grouping = child->property("Scene.ObjGroup");
		if (obj_grouping.isValid()) {
			QString group_name = obj_grouping.toString();
			removeSceneObjectFromGroup(qobject_cast<QEntity*>(child), group_name);
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

QCamera* SceneWidget::getCameraObj() {
	return camera;
}

void SceneWidget::update_orthographic_scale() {
	setCameraLens(camera->lens()->projectionType());
}

void SceneWidget::setClearColor(QColor color){
	qt3d_view->defaultFrameGraph()->setClearColor(color);
}

void SceneWidget::setDefaultClearColor(QColor color){
	default_clear_color = color;
}

void SceneWidget::setOffscreenRender(QObject* surface) {
	if (surface != nullptr) {
		QWindow* window = qobject_cast<QWindow*>(surface);
		qt3d_view->defaultFrameGraph()->setSurface(window);

		width = window->width();
		height = window->height();
		setCameraLens(camera->lens()->projectionType());	
	} else {
		width = qt3d_view->width();
		height = qt3d_view->height();
		setCameraLens(camera->lens()->projectionType());	

		qt3d_view->defaultFrameGraph()->setSurface(qobject_cast<QWindow*>(qt3d_view));
	}
}

Qt3DRender::QRenderCaptureReply* SceneWidget::requestFrameCapture() {
	if (render_capture == nullptr) {
		render_capture = new QRenderCapture();
		qt3d_view->activeFrameGraph()->setParent(render_capture);
		qt3d_view->setActiveFrameGraph(render_capture);
	}
	return render_capture->requestCapture();
}

