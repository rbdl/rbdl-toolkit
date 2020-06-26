#include "MotionMarkerExtention.h"
#include "toolkit_errors.h"
#include "util.h"

#include <iostream>

#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

MotionMarkerExtention::MotionMarkerExtention(unsigned int marker_count,
                                             QColor marker_color,
                                             float marker_size) : 
	marker_count(marker_count), 
	marker_color(marker_color), 
	marker_size(marker_size), 
	WrapperExtention()
{
	marker_plain_root  = new QEntity;
	marker_plain_root->setProperty("Scene.ObjGroup", QVariant("Mocap Marker"));
	max_time = 0.;
	marker_names.resize(marker_count);

	marker_mesh = new QMesh; 
	marker_mesh->setSource(QUrl::fromLocalFile(findFile(QString("unit_sphere_medres.obj"))));
}

std::string MotionMarkerExtention::getExtentionName() {
	return "MocapMarker";
}

void MotionMarkerExtention::update(float current_time) {
	if (marker_count == 0)
		return;

	unsigned int time_index = 0;
	for (; time_index < marker_times.size(); time_index++) {
		if (time_index == marker_times.size()-1) break;
		if (marker_times[time_index] <= current_time && marker_times[time_index+1] > current_time) {
			break;
		}
	}

	Matrix3fd pos_frame;
	if (time_index == marker_times.size()-1) {
		pos_frame = marker_positions[marker_positions.size()-1]; 
	} else {
		float start_time = marker_times[time_index];
		float end_time = marker_times[time_index+1];
		float time_fraction = (current_time - start_time) / (end_time - start_time);

		pos_frame = marker_positions[time_index] + time_fraction * 
			(marker_positions[time_index+1] - marker_positions[time_index]);
	}
	for (int i=0; i<marker_count;i++) {
		QVector3D trans;
		trans[0] = pos_frame(0,i);
		trans[1] = pos_frame(1,i);
		trans[2] = pos_frame(2,i);

		marker_transforms[i]->setTranslation(trans);
	}
}

QEntity* MotionMarkerExtention::getVisual() {
	if (marker_count == 0) 
		return nullptr;
	marker_entities.clear();

	for(int i=0;i<marker_count;i++) {
		marker_entities.push_back(std::move(new QEntity(marker_plain_root)));
		QEntity* m_entity = marker_entities[i];

		Qt3DExtras::QPhongAlphaMaterial* material = new Qt3DExtras::QPhongAlphaMaterial;
		material->setAmbient(marker_color);
		material->setAlpha((float)marker_color.alpha() / 255.);

		marker_transforms.push_back(std::move(new Qt3DCore::QTransform));
		Qt3DCore::QTransform* position = marker_transforms[i];
		position->setTranslation(QVector3D(marker_positions[0](0,i), marker_positions[0](1,i), marker_positions[0](2,i)));
		position->setScale(marker_size);

		m_entity->addComponent(marker_mesh);
		m_entity->addComponent(position);
		m_entity->addComponent(material);
	}

	return marker_plain_root;
}

void MotionMarkerExtention::addMocapMarkerFrame(float time, const Matrix3fd& marker_pos) {
	if (time > max_time) 
		max_time = time;

	marker_times.push_back(std::move(time));
	marker_positions.push_back(std::move(marker_pos));
}

void MotionMarkerExtention::setMarkerLabel(unsigned int pos, std::string& label) {
	if (pos<marker_count) 
		marker_names[pos] = label;
}
