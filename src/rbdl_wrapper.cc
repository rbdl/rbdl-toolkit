#include "rbdl_wrapper.h"
#include "rbdl/rbdl_errors.h"

#include <QFileInfo>
#include <QDir>
#include <QMatrix4x4>

#include <Qt3DCore/QTransform>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QAttribute>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>

#include "render_util.h"
#include "util.h"

using namespace RigidBodyDynamics;
using namespace RigidBodyDynamics::Math;

RBDLModelWrapper::RBDLModelWrapper(){
	model_render_obj = NULL;
	rbdl_model = NULL;
}


Qt3DCore::QEntity* RBDLModelWrapper::loadFromFile(QString model_file) {
	for ( auto loaded_extension : extension_names ) {
		delete extensions[loaded_extension];
	}
	extension_names.clear();
	extensions.clear();

	QFileInfo check_file(model_file);
	// Is it really a file and no directory?
	if (!check_file.exists() || !check_file.isFile()) {
		throw Errors::RBDLInvalidFileError("The file you tried to load does not exists!");
	}

	// some logic to save current pwd and move to the dir where the model is loaded from
	// needed so that lua model scripts with relative dependencies work correctly!
	auto last_pwd = QDir::currentPath();
	auto model_file_info = QFileInfo(model_file);
	auto model_pwd = model_file_info.absoluteDir().absolutePath();
	QDir::setCurrent(model_pwd);

	this->model_file = model_file;
	if (rbdl_model != NULL) {
		delete rbdl_model;
	}
	rbdl_model = new RigidBodyDynamics::Model();

	//loading model into rbdl to check its validity, may throw error
	try {
		RigidBodyDynamics::Addons::LuaModelReadFromFile(model_file.toStdString().c_str(), rbdl_model, false);
		//load model lua extra to read parameters for rendering
		model_luatable = LuaTable::fromFile(model_file.toStdString().c_str());
		//std::cout << model_luatable.serialize() << std::endl;
	} catch (Errors::RBDLError& err) {
		// return to original pwd
		QDir::setCurrent(last_pwd);
		// send error up the stack 
		throw err;
	}
	auto q = VectorNd::Zero(rbdl_model->q_size);
	if (model_render_obj != NULL) {
		delete model_render_obj;
	}
	model_render_obj = new Qt3DCore::QEntity();
	auto model_obj = new Qt3DCore::QEntity(model_render_obj);

	//read axes form model file
	Vector3d axis_front = model_luatable["configuration"]["axis_front"].getDefault(Vector3d(1., 0., 0.)); 
	Vector3d axis_up = model_luatable["configuration"]["axis_up"].getDefault(Vector3d(0., 1., 0.));
	Vector3d axis_right = model_luatable["configuration"]["axis_right"].getDefault(Vector3d(0., 0., 1.));

	//measurement unit used by meshes
	std::string mesh_unit = model_luatable["configuration"]["mesh_measurement_unit"].getDefault(std::string("m"));
	if (mesh_unit == "cm") {
			mesh_unit_scaling = 0.01;
	} else if (mesh_unit == "mm") {
			mesh_unit_scaling = 0.001;
	// assume meters
	} else {
		mesh_unit_scaling = 1.;
	}

	unsigned int segments_cnt = model_luatable["frames"].length();

	//create renderable entities for every segment of the model
	for (int i=1; i<=segments_cnt ; i++) {
		std::string segment_name = model_luatable["frames"][i]["name"].get<std::string>();

		Qt3DCore::QEntity* segment_render_node = new Qt3DCore::QEntity();

		//every segment can render multiple visuals
		unsigned int visuals_cnt = model_luatable["frames"][i]["visuals"].length();

		for (int j=1; j<=visuals_cnt; j++) {
			//Wrapper Entity 
			Qt3DCore::QEntity* visual_entity = new Qt3DCore::QEntity(segment_render_node);
			visual_entity->setProperty("Scene.ObjGroup", QVariant(QString("Segments"))); 

			//Mesh Entity -> needed because extra transform may be applied to it
			Qt3DCore::QEntity* mesh_entity = new Qt3DCore::QEntity(visual_entity);

			//Mesh Transforms 
			Qt3DCore::QTransform* mesh_transform = new Qt3DCore::QTransform;

			if(model_luatable["frames"][i]["visuals"][j]["rotate"].exists()) {
				float angle = model_luatable["frames"][i]["visuals"][j]["rotate"]["angle"].getDefault(0.f);
				Vector3d axis = model_luatable["frames"][i]["visuals"][j]["rotate"]["axis"].getDefault(Vector3d(1., 0., 0.));
				axis = axis.normalized();
				auto rotation = QQuaternion::fromAxisAndAngle(QVector3D(axis[0], axis[1], axis[2]), angle);
				mesh_transform->setRotation(rotation);
			}

			Vector3d visual_center = model_luatable["frames"][i]["visuals"][j]["translate"].getDefault(Vector3d(0., 0., 0.));
			mesh_transform->setTranslation(QVector3D(visual_center[0], visual_center[1], visual_center[2]));

			//Material Properties
			Vector3d visual_color = model_luatable["frames"][i]["visuals"][j]["color"].getDefault(Vector3d(1., 1., 1.));
			float visual_alpha = model_luatable["frames"][i]["visuals"][j]["alpha"].getDefault(1.);
			Qt3DExtras::QDiffuseSpecularMaterial* visual_material = new Qt3DExtras::QDiffuseSpecularMaterial;
			if (visual_alpha < 0.7) 
				visual_material->setAlphaBlendingEnabled(true);
			visual_material->setAmbient(QColor::fromRgbF(visual_color[0], visual_color[1], visual_color[2], visual_alpha));

			//Mesh src
			std::string src = model_luatable["frames"][i]["visuals"][j]["src"].get<std::string>();
			QString visual_mesh_src = findFile(src, true);
			Qt3DRender::QMesh* visual_mesh = new Qt3DRender::QMesh;
			visual_mesh->setSource(QUrl::fromLocalFile(visual_mesh_src));

			//Mesh Entity completed
			mesh_entity->addComponent(visual_mesh);
			mesh_entity->addComponent(mesh_transform);
			mesh_entity->addComponent(visual_material);

			//Wrapper Entity 
			Vector3d visual_scale = model_luatable["frames"][i]["visuals"][j]["scale"].getDefault(Vector3d(1., 1., 1.));
			visual_scale = ( mesh_unit_scaling * visual_scale);
			Vector3d visual_dimensions = model_luatable["frames"][i]["visuals"][j]["dimensions"].getDefault(Vector3d(1., 1., 1.));
			Vector3d visual_translate = model_luatable["frames"][i]["visuals"][j]["mesh_center"].getDefault(Vector3d(0., 0., 0.));

			Qt3DCore::QTransform* visual_transform = new Qt3DCore::QTransform;
			visual_transform->setScale3D(QVector3D(visual_dimensions[0] * visual_scale[0], visual_dimensions[1] * visual_scale[1], visual_dimensions[2] * visual_scale[2]));
			visual_transform->setTranslation(QVector3D(visual_translate[0], visual_translate[1], visual_translate[2]));

			visual_entity->addComponent(visual_transform);
		}

		//calculate position and orientation of segment to the model root
		unsigned int body_id = rbdl_model->GetBodyId(segment_name.c_str());
		auto segment_spacial_transform = CalcBodyToBaseCoordinates(*rbdl_model, q, body_id, Vector3d(0., 0., 0.));
		segment_spacial_transform = segment_spacial_transform;
		auto segment_rotation = Quaternion::fromMatrix(CalcBodyWorldOrientation(*rbdl_model, q, body_id));

		Qt3DCore::QTransform* segment_transform = new Qt3DCore::QTransform;
		segment_transform->setTranslation(QVector3D(segment_spacial_transform[0], segment_spacial_transform[1], segment_spacial_transform[2]));
		segment_transform->setRotation(QQuaternion(segment_rotation[3], segment_rotation[0], segment_rotation[1], segment_rotation[2]));

		//std::cout << segment_spacial_transform.transpose() << std::endl;

		segment_render_node->addComponent(segment_transform);
		segment_render_node->setParent(model_render_obj);

		body_mesh_map[segment_name] = segment_render_node;
		body_transform_map[segment_name] = segment_transform;
	}

	auto model_spacial_transform = CalcBodyToBaseCoordinates(*rbdl_model, q, 0, Vector3d(0., 0., 0.));
	auto model_spacial_rotation = Quaternion::fromMatrix(CalcBodyWorldOrientation(*rbdl_model, q, 0));

	//add a constant rotation for rotating object to fit opengl coordinates
	auto rotation = QQuaternion(model_spacial_rotation[3], model_spacial_rotation[0], model_spacial_rotation[1], model_spacial_rotation[2]);

	Qt3DCore::QTransform* model_transform = new Qt3DCore::QTransform;
	model_transform->setRotation(rotation);
	model_transform->setTranslation(QVector3D(model_spacial_transform[0], model_spacial_transform[1], model_spacial_transform[2]));
	model_obj->addComponent(model_transform);

	Qt3DCore::QTransform* world_axis_transform = new Qt3DCore::QTransform;
	QMatrix4x4 m(axis_right[0], axis_right[1], axis_right[2], 0.,
	             axis_up[0], axis_up[1], axis_up[2], 0.,
	             axis_front[0], axis_front[1], axis_front[2], 0.,
	             0., 0., 0., 1.
	            );
	world_axis_transform->setMatrix(m);
	model_render_obj->addComponent(world_axis_transform);

	//return to original pwd
	QDir::setCurrent(last_pwd);

	return model_render_obj;
}

void RBDLModelWrapper::updateKinematics(RigidBodyDynamics::Math::VectorNd Q) {
	for (auto it = body_transform_map.begin(); it != body_transform_map.end(); it++) {
		int body_id = rbdl_model->GetBodyId(it->first.c_str());
		
		auto segment_spacial_transform = CalcBodyToBaseCoordinates(*rbdl_model, Q, body_id, Vector3d(0., 0., 0.), true);
		segment_spacial_transform = segment_spacial_transform;
		auto segment_rotation = Quaternion::fromMatrix(CalcBodyWorldOrientation(*rbdl_model, Q, body_id, true));

		Qt3DCore::QTransform* segment_transform = it->second;
		segment_transform->setTranslation(QVector3D(segment_spacial_transform[0], segment_spacial_transform[1], segment_spacial_transform[2]));
		segment_transform->setRotation(QQuaternion(segment_rotation[3], segment_rotation[0], segment_rotation[1], segment_rotation[2]));
	}
}

void RBDLModelWrapper::reload() {
	this->loadFromFile(this->model_file);
}

void RBDLModelWrapper::model_update(float current_time) {
	for (auto it = extensions.begin(); it != extensions.end(); it++) {
		WrapperExtension* extension = it->second;
		extension->update(current_time);
	}
}

void RBDLModelWrapper::addExtension(WrapperExtension* extension) {
	std::string extension_name = extension->getExtensionName();
	extensions[extension_name] = extension;
	extension_names.push_back(extension_name);
	extension->setModelParent(this);

	Qt3DCore::QEntity* visual = extension->getVisual();
	if (visual != nullptr) {
		visual->setParent(model_render_obj);
		emit visual_added(visual);
	}

	emit new_extension_added();
}

bool RBDLModelWrapper::hasExtension(std::string name) {
	for ( auto loaded_extension : extension_names ) {
		if (loaded_extension == name ) {
			return true;
		}
	}
	return false;
}

WrapperExtension* RBDLModelWrapper::getExtension(std::string name) {
	for ( auto loaded_extension : extension_names ) {
		if (loaded_extension == name ) {
			return extensions[name];
		}
	}
	return nullptr;
}

int RBDLModelWrapper::getModelDof() {
	return rbdl_model->dof_count;
}

QString RBDLModelWrapper::getModelFile() {
	return model_file;
}

void RBDLModelWrapper::addStaticVisual(std::string segment_name, Qt3DCore::QEntity *visual) {
	Qt3DCore::QEntity* segment_entity;
	try {
		segment_entity = body_mesh_map.at(segment_name);
	} catch (std::exception &e){
		return;
	}
	visual->setParent(segment_entity);
	emit visual_added(visual);
}

void WrapperExtension::setModelParent(RBDLModelWrapper* model) {
	model_parent = model;
}

Qt3DCore::QEntity* WrapperExtension::getVisual() {
	return nullptr;
}

WrapperExtension::WrapperExtension() {
	model_parent = NULL;
}

