#include "ModelMarkerExtension.h"
#include "toolkit_errors.h"
#include "util.h"

#include <iostream>

#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

ModelMarkerExtension::ModelMarkerExtension(QColor marker_color, float marker_size) : 
	marker_color(marker_color), 
	marker_size(marker_size), 
	WrapperExtension()
{
	marker_mesh = new QMesh; 
	marker_mesh->setSource(QUrl::fromLocalFile(findFile(QString("unit_sphere_medres.obj"))));
}

std::string ModelMarkerExtension::getExtensionName() {
	return "ModelMarker";
}

void ModelMarkerExtension::update(float current_time) {
	//This extention only adds static objects to the model, no in sync updates needed
	return;
}

QEntity* ModelMarkerExtension::getVisual() {
	if (model_parent != nullptr) {
		for (int i=0;i<marker_names.size();i++) {
			marker_visuals.push_back(new QEntity);
			Qt3DCore::QEntity* marker_visual = marker_visuals[marker_visuals.size()-1];
			marker_visual->setProperty("Scene.ObjGroup", QVariant("Model Marker"));


			Qt3DExtras::QPhongAlphaMaterial* material = new Qt3DExtras::QPhongAlphaMaterial;
			material->setAmbient(marker_color);
			material->setAlpha((float)marker_color.alpha() / 255.);

			Qt3DCore::QTransform* position = new Qt3DCore::QTransform;
			position->setTranslation(QVector3D(marker_positions[i][0], marker_positions[i][1], marker_positions[i][2]));
			position->setScale(marker_size);

			marker_visual->addComponent(marker_mesh);
			marker_visual->addComponent(position);
			marker_visual->addComponent(material);

			model_parent->addStaticVisual(marker_segments[i], marker_visual);
		}
	}
	//Visuals are added as static objects to the model, without using a single root
	return nullptr;
}

void ModelMarkerExtension::addModelMarker(const std::string& label, const std::string& segment_name, const Vector3d& position) {
	marker_names.push_back(label);
	marker_segments.push_back(segment_name);
	marker_positions.push_back(position);
}
