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
#include "rbdl_wrapper.h"
#include "luamodel_wrapper.h"
#include "rbdl/rbdl_errors.h"

using namespace RigidBodyDynamics;
using namespace RigidBodyDynamics::Math;

RBDLModelWrapper::RBDLModelWrapper(){
	model_root = NULL;
	rbdl_model = NULL;
}


RBDLModelWrapper* RBDLModelWrapper::loadFromFile(QString model_file) {
	QFileInfo check_file(model_file);
	// Is it really a file and no directory?
	if (!check_file.exists() || !check_file.isFile()) {
		throw Errors::RBDLInvalidFileError("The file you tried to load does not exists!");
	}

	// todo logic to detect model type and load with correct subclass
	RBDLModelWrapper* model_wrapper;
	if (check_file.suffix() == "lua") {
		model_wrapper = new LuaModelWrapper();
	} else {
		throw Errors::RBDLInvalidFileError("The model file you tried to load is not supported!");
	}
	model_wrapper->load(model_file);

	return model_wrapper;
}

Qt3DCore::QEntity* RBDLModelWrapper::getSegmentEntity(std::string segment_name, bool create) {
	//find segment entity 
	for (auto it = body_segment_map.begin(); it != body_segment_map.end(); it++) {
		if (it->first == segment_name) {
			return it->second;
		}
	}
	// if segment_entity not found and create enable create a new model segment
	if (create) {
		Qt3DCore::QEntity* segment_render_node = new Qt3DCore::QEntity();
		segment_render_node->setProperty("Scene.ObjGroup", QVariant(QString("Segments"))); 

		//calculate position and orientation of segment to the model root
		unsigned int body_id = rbdl_model->GetBodyId(segment_name.c_str());
		auto segment_spacial_transform = CalcBodyToBaseCoordinates(*rbdl_model, current_Q, body_id, Vector3d(0., 0., 0.));
		auto segment_rotation = Quaternion::fromMatrix(CalcBodyWorldOrientation(*rbdl_model, current_Q, body_id));

		Qt3DCore::QTransform* segment_transform = new Qt3DCore::QTransform;
		segment_transform->setTranslation(QVector3D(segment_spacial_transform[0], segment_spacial_transform[1], segment_spacial_transform[2]));
		segment_transform->setRotation(QQuaternion(segment_rotation[3], segment_rotation[0], segment_rotation[1], segment_rotation[2]));

		segment_render_node->addComponent(segment_transform);
		segment_render_node->setParent(model_root);

		body_segment_map[segment_name] = segment_render_node;
		body_transform_map[segment_name] = segment_transform;

		return segment_render_node;
	}

	throw RigidBodyDynamics::Errors::RBDLError(QString("Segment with name %1 was not found it this model!").arg(QString::fromStdString(segment_name)).toStdString());
}

void RBDLModelWrapper::build3DEntity(ModelInfo& model_info, std::vector<SegmentVisualInfo>& visuals_info) {
	current_Q = VectorNd::Zero(rbdl_model->q_size);
	if (model_root != NULL) {
		delete model_root;
	}
	model_root = new Qt3DCore::QEntity();
	auto model_obj = new Qt3DCore::QEntity(model_root);

	for (auto vis_info : visuals_info) {
		auto segment_render_node = getSegmentEntity(vis_info.segment_name, true);
		//Visual Entity 
		Qt3DCore::QEntity* visual_entity = new Qt3DCore::QEntity(segment_render_node);

		//Mesh Entity -> needed for correct order of transforms
		createMeshEntity(vis_info.mesh_file, vis_info.visual_color, vis_info.mesh_translation, vis_info.mesh_rotation, visual_entity);

		Qt3DCore::QTransform* visual_transform = new Qt3DCore::QTransform;
		visual_transform->setScale3D(vis_info.visual_dimensions * model_info.unit_scaling);
		visual_transform->setTranslation(vis_info.visual_center);

		visual_entity->addComponent(visual_transform);
	}

	auto model_spacial_transform = CalcBodyToBaseCoordinates(*rbdl_model, current_Q, 0, Vector3d(0., 0., 0.));
	auto model_spacial_rotation = Quaternion::fromMatrix(CalcBodyWorldOrientation(*rbdl_model, current_Q, 0));
	auto rotation = QQuaternion(model_spacial_rotation[3], model_spacial_rotation[0], model_spacial_rotation[1], model_spacial_rotation[2]);

	Qt3DCore::QTransform* model_transform = new Qt3DCore::QTransform;
	model_transform->setRotation(rotation);
	model_transform->setTranslation(to_qt_vector(model_spacial_transform));
	model_obj->addComponent(model_transform);

	Qt3DCore::QTransform* world_axis_transform = new Qt3DCore::QTransform;
	world_axis_transform->setMatrix(model_info.model_world_transform);
	model_root->addComponent(world_axis_transform);
}

void RBDLModelWrapper::updateKinematics(RigidBodyDynamics::Math::VectorNd Q) {
	current_Q = Q;
	for (auto it = body_transform_map.begin(); it != body_transform_map.end(); it++) {
		int body_id = rbdl_model->GetBodyId(it->first.c_str());
		
		auto segment_spacial_transform = CalcBodyToBaseCoordinates(*rbdl_model, Q, body_id, Vector3d(0., 0., 0.), true);
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
		visual->setParent(model_root);
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
		segment_entity = body_segment_map.at(segment_name);
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

