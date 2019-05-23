/* ------------------------------------------------------------------------
 *  Static Function for adding some basic things to the scene
 */
#include "render_util.h"

#include <Qt3DCore/QTransform>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DExtras/QPhongMaterial>

#include <cmath>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

QEntity* loadFromLuaTable(LuaTableNode &table) {
	//Todo
}

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

