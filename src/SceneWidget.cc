#include "SceneWidget.h"

#include <Qt3DCore/QTransform>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QGeometry>

#include <Qt3DInput/QInputAspect>

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>

#include <cmath>

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

	qt3d_view->setRootEntity(scene_root);

	display_container = new QHBoxLayout(this);
	display_container->setMargin(0);
	display_container->addWidget(QWidget::createWindowContainer(qt3d_view));
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



/* ------------------------------------------------------------------------
 *  Static Function for adding some basic things to the scene
 */


QEntity* createGridFloor(float lborder, float rborder, int count, QColor line_color) {
	QGeometry* floor_geometry = new Qt3DRender::QGeometry();
	float step = fabs (rborder - lborder) / (float) count;
	QByteArray vertexBytes;
	// buffer size:		(x and z) * (count of lines) * (start end) * (vertex size) * (data type)
	vertexBytes.resize( 2 * (count+1) * 2 * 3 * sizeof(float) );
	float* data_ptr = reinterpret_cast<float*> (vertexBytes.data());
	for (int i=0 ; i<=count ; i++ ) {
		//x start
		*data_ptr++ = (i * step) + lborder;
		*data_ptr++ = 0.;
		*data_ptr++ = lborder;
		//x end
		*data_ptr++ = (i * step) + lborder;
		*data_ptr++ = 0.;
		*data_ptr++ = rborder;

		//z start
		*data_ptr++ = lborder;
		*data_ptr++ = 0.;
		*data_ptr++ = (i * step) + lborder;
		//z end
		*data_ptr++ = rborder;
		*data_ptr++ = 0.;
		*data_ptr++ = (i * step) + lborder;
	}

	QBuffer* grid_vertexes = new QBuffer(floor_geometry);
	grid_vertexes->setUsage(QBuffer::StaticDraw);
	grid_vertexes->setData(vertexBytes);

	auto *vertexAttribute = new Qt3DRender::QAttribute(floor_geometry);
	vertexAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
	vertexAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
	vertexAttribute->setVertexSize(3);
	vertexAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
	vertexAttribute->setBuffer(grid_vertexes);
	vertexAttribute->setByteStride(3 * sizeof(float));
	vertexAttribute->setCount(2 * 2 * (count+1));
	floor_geometry->addAttribute(vertexAttribute); // We add the vertices in the geometry

	// connectivity between vertices
	QByteArray indexBytes;
	indexBytes.resize(2 * 2 * (count+1) * sizeof(unsigned int)); // start to end
	unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());
	for (int i=0; i < 4*(count+1); i++) {
		*indices++ = i;
	}
	auto *indexBuffer = new Qt3DRender::QBuffer(floor_geometry);
	indexBuffer->setData(indexBytes);

	auto *indexAttribute = new Qt3DRender::QAttribute(floor_geometry);
	indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
	indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
	indexAttribute->setBuffer(indexBuffer);
	indexAttribute->setCount(4*(count+1));
	floor_geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

	auto *floor_lines = new Qt3DRender::QGeometryRenderer;
	floor_lines->setGeometry(floor_geometry);
	floor_lines->setPrimitiveType(QGeometryRenderer::Lines);

	Qt3DCore::QTransform* transform = new Qt3DCore::QTransform;
	transform->setTranslation(QVector3D(0.0, 0.0, 0.0));

	Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial;
	material->setAmbient(line_color);

	QEntity* floor = new QEntity;

	floor->addComponent(transform);
	floor->addComponent(floor_lines);
	floor->addComponent(material);

	floor->setProperty("Scene.ObjGroup", QVariant("Grid"));

	return floor;
}

QEntity* loadMeshFromFile(const char* filename) {
	//Todo
}
