/* ------------------------------------------------------------------------
 *  Static Function for adding some basic things to the scene
 */

/*
 * Between Qt5 and Qt6 certain Qt3D Classes changed their namespace
 * this hack detects what version is used and sets the correct namespace
 */
#ifdef USE_QT6
  #include <Qt3DCore/QAttribute>
  #include <Qt3DCore/QBuffer>
  #include <Qt3DCore/QGeometry>
#else
  #include <Qt3DRender/QAttribute>
  #include <Qt3DRender/QBuffer>
  #include <Qt3DRender/QGeometry>
#endif //USE_QT_6

#include "render_util.h"

#include <Qt3DCore/QTransform>


#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QLineWidth>

#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QCylinderMesh>

#include <cmath>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

TOOLKIT_DLLAPI QEntity* createGridFloor(float lborder, float rborder, int count, QColor line_color) {
	QGeometry* floor_geometry = new QGeometry();
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

	#if QT_VERSION > 0x50a00
		QBuffer* grid_vertexes = new QBuffer(floor_geometry);
	#else
		QBuffer* grid_vertexes = new QBuffer(Qt3DCore::QBuffer::VertexBuffer, floor_geometry);
	#endif

	grid_vertexes->setUsage(QBuffer::StaticDraw);
	grid_vertexes->setData(vertexBytes);

	auto *vertexAttribute = new QAttribute(floor_geometry);
	vertexAttribute->setName(QAttribute::defaultPositionAttributeName());
	vertexAttribute->setVertexBaseType(QAttribute::Float);
	vertexAttribute->setVertexSize(3);
	vertexAttribute->setAttributeType(QAttribute::VertexAttribute);
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

	#if QT_VERSION > 0x50a00
		auto *indexBuffer = new QBuffer(floor_geometry);
	#else
		auto *indexBuffer = new QBuffer(QBuffer::IndexBuffer, floor_geometry);
	#endif
	indexBuffer->setData(indexBytes);

	auto *indexAttribute = new QAttribute(floor_geometry);
	indexAttribute->setVertexBaseType(QAttribute::UnsignedInt);
	indexAttribute->setAttributeType(QAttribute::IndexAttribute);
	indexAttribute->setBuffer(indexBuffer);
	indexAttribute->setCount(4*(count+1));
	floor_geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

	auto *floor_lines = new Qt3DRender::QGeometryRenderer;
	floor_lines->setGeometry(floor_geometry);
	floor_lines->setPrimitiveType(QGeometryRenderer::Lines);

	Qt3DCore::QTransform* transform = new Qt3DCore::QTransform;
	transform->setTranslation(QVector3D(0.0, 0.0, 0.0));

	Qt3DExtras::QDiffuseSpecularMaterial *material = new Qt3DExtras::QDiffuseSpecularMaterial;
	material->setAmbient(line_color);

	QEntity* floor = new QEntity;

	floor->addComponent(transform);
	floor->addComponent(floor_lines);
	floor->addComponent(material);

	floor->setProperty("Scene.ObjGroup", QVariant("Grid"));

	return floor;
}

TOOLKIT_DLLAPI Qt3DCore::QEntity* createWire(const QVector3D& direction, const QColor& line_color, float line_width, Qt3DCore::QEntity* parent) {
	auto rot = QQuaternion::rotationTo(QVector3D(0.0, 1.0, 0.0), direction);
	auto trans = QVector3D(0.0, 1*direction.length()/2, 0.0);
	Qt3DCore::QTransform* transform = new Qt3DCore::QTransform;
	transform->setTranslation(rot*trans);
	transform->setRotation(rot);

	Qt3DExtras::QDiffuseSpecularMaterial *material = new Qt3DExtras::QDiffuseSpecularMaterial;
	material->setAmbient(line_color);

	Qt3DExtras::QCylinderMesh *wire_mesh = new Qt3DExtras::QCylinderMesh;
	wire_mesh->setLength(direction.length());
	wire_mesh->setRadius(line_width/2);


	QEntity* wire;
	if (parent == nullptr) {
		wire = new Qt3DCore::QEntity();
	} else {
		wire = new Qt3DCore::QEntity(parent);
	}

	wire->addComponent(transform);
	wire->addComponent(material);
	wire->addComponent(wire_mesh);

	return wire;
}

TOOLKIT_DLLAPI Qt3DCore::QEntity* createMeshEntity(const QString& mesh_file,
                                    const QColor& mesh_color, 
                                    const QVector3D& mesh_translation, 
                                    const QQuaternion& mesh_rotation,
                                    Qt3DCore::QEntity* parent) {
	Qt3DCore::QEntity* mesh_entity;
	if (parent == nullptr) {
		mesh_entity = new Qt3DCore::QEntity();
	} else {
		mesh_entity = new Qt3DCore::QEntity(parent);
	}

	//Mesh Transforms 
	Qt3DCore::QTransform* mesh_transform = new Qt3DCore::QTransform;
	mesh_transform->setRotation(mesh_rotation);
	mesh_transform->setTranslation(mesh_translation);

	//Mesh Material
	Qt3DExtras::QDiffuseSpecularMaterial* visual_material = new Qt3DExtras::QDiffuseSpecularMaterial;
	//if (mesh_color.alpha() < 0.7)
	//	visual_material->setAlphaBlendingEnabled(true);
	visual_material->setAmbient(mesh_color);

	//Mesh src
	Qt3DRender::QMesh* visual_mesh = new Qt3DRender::QMesh;
	visual_mesh->setSource(QUrl::fromLocalFile(mesh_file));

	//Mesh Entity completed
	mesh_entity->addComponent(visual_mesh);
	mesh_entity->addComponent(mesh_transform);
	mesh_entity->addComponent(visual_material);

	mesh_entity->setProperty("Scene.ObjGroup", QVariant("Meshes"));

	return mesh_entity;
}

