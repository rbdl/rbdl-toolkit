#include "VisualExtrasPlugin.h"
#include "render_util.h"

#include <iostream>

#include <rbdl/rbdl.h>
#include <rbdl/rbdl_math.h>
using namespace RigidBodyDynamics;
using namespace RigidBodyDynamics::Math;

VisualExtrasPlugin::VisualExtrasPlugin() {
	parentApp = NULL;
}

VisualExtrasPlugin::~VisualExtrasPlugin() {
}

void VisualExtrasPlugin::init(ToolkitApp* app) {
	//save reference to parent ToolkitApp 
	parentApp = app;

	loadVisualExtrasSettings();

	connect(parentApp, &ToolkitApp::model_loaded, this, &VisualExtrasPlugin::reload);
	connect(parentApp, &ToolkitApp::reloaded_model, this, &VisualExtrasPlugin::reload);
}

void VisualExtrasPlugin::loadVisualExtrasSettings() {
	parentApp->toolkit_settings.beginGroup("RenderOptions");

	QVariant val = parentApp->toolkit_settings.value("modeltree.color");
	if (val.isNull()) {
		modeltree_color = QColor::fromRgbF(1., 1., 0., 0.);
		parentApp->toolkit_settings.setValue("modeltree.color", modeltree_color.rgba());
	} else {
		modeltree_color = QColor::fromRgba(val.toUInt());
	}
	parentApp->toolkit_settings.setType("modeltree.color", modeltree_color);

	val = parentApp->toolkit_settings.value("modeltree.size");
	if (val.isNull()) {
		modeltree_size = 0.001;
		parentApp->toolkit_settings.setValue("modeltree.size", modeltree_size);
	} else {
		modeltree_size = val.toFloat();
	}
	parentApp->toolkit_settings.setType("modeltree.size", modeltree_size);

	parentApp->toolkit_settings.endGroup();
}

void VisualExtrasPlugin::buildModelTreeWireframe(RBDLModelWrapper* model) {
	for (auto it = model->rbdl_model->mBodyNameMap.begin(); it != model->rbdl_model->mBodyNameMap.end(); it++) {
		std::string segment_name = it->first;
		int body_id = it->second;

		auto current_Q = VectorNd::Zero(model->rbdl_model->q_size);
		int parent_body_id = model->rbdl_model->GetParentBodyId(body_id);
		auto parent_body_name = model->rbdl_model->GetBodyName(parent_body_id);

		auto segment_render_node = model->getSegmentEntity(parent_body_name, true);

		auto body_pos = CalcBodyToBaseCoordinates(*model->rbdl_model,
												  current_Q,
												  body_id,
												  Vector3d(0., 0., 0.));
		auto parent_body_pos = CalcBodyToBaseCoordinates(*model->rbdl_model,
														 current_Q,
														 parent_body_id,
														 Vector3d(0., 0., 0.));
		auto parent_orientation = CalcBodyWorldOrientation(*model->rbdl_model,
														   current_Q,
														   parent_body_id);

		auto body_translation = parent_orientation * (body_pos - parent_body_pos);
		auto wire_entity = createWire(QVector3D(
									  body_translation.x(),
									  body_translation.y(),
									  body_translation.z()),
									  modeltree_color,
									  modeltree_size,
									  segment_render_node);
		wire_entity->setProperty("Scene.ObjGroup", QVariant("ModelTree"));
	}
}




void VisualExtrasPlugin::reload(RBDLModelWrapper *model) {
	loadVisualExtrasSettings();
	buildModelTreeWireframe(model);
}
