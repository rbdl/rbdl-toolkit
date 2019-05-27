#include "rbdl_wrapper.h"

#include <QFileInfo>

#include <Qt3DCore/QTransform>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QAttribute>
#include <Qt3DExtras/QPhongMaterial>

#include "render_util.h"

using namespace RigidBodyDynamics;
using namespace RigidBodyDynamics::Math;

RBDLModelWrapper::RBDLModelWrapper(){
	model_render_obj = NULL;
}

Qt3DCore::QEntity* RBDLModelWrapper::loadFromFile(QString model_file) {

	QFileInfo check_file(model_file);
	// Is it really a file and no directory?
	if (!check_file.exists() || !check_file.isFile()) {
		//Todo raise error
	}

	this->model_file = model_file;

	//try loading model into rbdl to check its validity
	if (!RigidBodyDynamics::Addons::LuaModelReadFromFile(model_file.toStdString().c_str(), &rbdl_model, false)) {
		//Todo raise error
	}

	auto q = VectorNd::Zero(rbdl_model.q_size);
	model_render_obj = new Qt3DCore::QEntity();

	// load model lua extra to read parameters for rendering
	model_luatable = LuaTable::fromFile(model_file.toStdString().c_str());
	std::cout << model_luatable.serialize() << std::endl;

	unsigned int segments_cnt = model_luatable["frames"].length();

	//create renderable entities for every segment of the model
	for (int i=1; i<=segments_cnt ; i++) {
		std::string segment_name = model_luatable["frames"][i]["name"].get<std::string>();

		Qt3DCore::QEntity* segment_render_node = new Qt3DCore::QEntity();

		//every segment can render multiple visuals
		unsigned int visuals_cnt = model_luatable["frames"][i]["visuals"].length();

		for (int j=1; j<=visuals_cnt; j++) {

			std::string visual_mesh_src = model_luatable["frames"][i]["visuals"][j]["src"].get<std::string>();

			Vector3d visual_color = model_luatable["frames"][i]["visuals"][j]["color"].getDefault<Vector3d>(Vector3d(1., 1., 1.));
			Vector3d visual_dimensions = model_luatable["frames"][i]["visuals"][j]["dimensions"].getDefault<Vector3d>(Vector3d(1., 1., 1.));
			Vector3d visual_center = model_luatable["frames"][i]["visuals"][j]["mesh_center"].getDefault<Vector3d>(Vector3d(0., 0., 0.));

			Qt3DCore::QEntity* visual_entity = new Qt3DCore::QEntity(segment_render_node);

			Qt3DCore::QTransform* visual_transform = new Qt3DCore::QTransform;
			visual_transform->setTranslation(QVector3D(visual_center[0], visual_center[1], visual_center[2]));
			visual_transform->setScale3D(QVector3D(visual_dimensions[0], visual_dimensions[1], visual_dimensions[2]));

			Qt3DExtras::QPhongMaterial* visual_material = new Qt3DExtras::QPhongMaterial;
			visual_material->setAmbient(QColor::fromRgbF(visual_color[0], visual_color[1], visual_color[2], 1.));

			Qt3DRender::QMesh* visual_mesh = new Qt3DRender::QMesh;
			visual_mesh->setSource(QUrl::fromLocalFile(QString::fromStdString(visual_mesh_src)));

			visual_entity->addComponent(visual_transform);
			visual_entity->addComponent(visual_material);
			visual_entity->addComponent(visual_mesh);
		}


		unsigned int body_id = rbdl_model.GetBodyId(segment_name.c_str());
		auto segment_spacial_transform = CalcBodyToBaseCoordinates(rbdl_model, q, body_id, Vector3d(0., 0., 0.));
		auto segment_rotation = Quaternion::fromMatrix(CalcBodyWorldOrientation(rbdl_model, q, body_id));

		Qt3DCore::QTransform* segment_transform = new Qt3DCore::QTransform;
		segment_transform->setTranslation(QVector3D(segment_spacial_transform[0], segment_spacial_transform[1], segment_spacial_transform[2]));
		segment_transform->setRotation(QQuaternion(segment_rotation[0], segment_rotation[1], segment_rotation[2], segment_rotation[3]));

		segment_render_node->addComponent(segment_transform);
		segment_render_node->setParent(model_render_obj);

		body_mesh_map[segment_name] = segment_render_node;
	}

	auto model_spacial_transform = CalcBodyToBaseCoordinates(rbdl_model, q, 0, Vector3d(0., 0., 0.));
	auto model_rotation = Quaternion::fromMatrix(CalcBodyWorldOrientation(rbdl_model, q, 0));

	Qt3DCore::QTransform* model_transform = new Qt3DCore::QTransform;
	model_transform->setTranslation(QVector3D(model_spacial_transform[0], model_spacial_transform[1], model_spacial_transform[2]));
	model_transform->setRotation(QQuaternion(model_rotation[0], model_rotation[1], model_rotation[2], model_rotation[3]));

	model_render_obj->addComponent(model_transform);

	model_render_obj->setProperty("Scene.ObjGroup", QVariant("Model"));

	return model_render_obj;
}
