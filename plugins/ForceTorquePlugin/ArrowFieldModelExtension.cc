#include "ArrowFieldModelExtension.h"
#include "toolkit_errors.h"

#include <iostream>

#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

ArrowFieldModelExtension::ArrowFieldModelExtension(
	QString arrow_mesh_path, 
	QString group_name,
	QColor arrow_color,
	float draw_threshold,
	float arrow_scale_factor ) : 
		max_time(0.),
		arrow_count(0),
		arrow_color(arrow_color),
		group_name(group_name),
		draw_threshold(draw_threshold),
		arrow_scale_factor(arrow_scale_factor),
		WrapperExtension()
{
	arrow_mesh = new QMesh; 
	arrow_mesh->setSource(QUrl::fromLocalFile(arrow_mesh_path));

	arrow_field_root = new QEntity;
	arrow_field_root->setProperty("Scene.ObjGroup", QVariant(group_name));
}

std::string ArrowFieldModelExtension::getExtensionName() {
	return group_name.toStdString();
}

void ArrowFieldModelExtension::update(float current_time) {
	if (arrow_positions.size() == 0 || arrow_count == 0)
		return;

	unsigned int time_index = 0;
	for (; time_index < arrow_times.size(); time_index++) {
		if (time_index == arrow_times.size()-1) break;
		if (arrow_times[time_index] <= current_time && arrow_times[time_index+1] > current_time) {
			break;
		}
	}

	Matrix3fd pos_frame;
	Matrix3fd dir_frame;
	if (time_index == arrow_times.size()-1) {
		pos_frame = arrow_positions[arrow_positions.size()-1]; 
		dir_frame = arrow_directions[arrow_directions.size()-1]; 
	} else {
		float start_time = arrow_times[time_index];
		float end_time = arrow_times[time_index+1];
		float time_fraction = (current_time - start_time) / (end_time - start_time);

		pos_frame = arrow_positions[time_index] + time_fraction * 
			(arrow_positions[time_index+1] - arrow_positions[time_index]);

		dir_frame = arrow_directions[time_index] + time_fraction * 
			(arrow_directions[time_index+1] - arrow_directions[time_index]);
	}
	for (int i=0; i<arrow_count;i++) {
		QVector3D trans;
		trans[0] = pos_frame(0,i);
		trans[1] = pos_frame(1,i);
		trans[2] = pos_frame(2,i);

		QVector3D direction;
		direction[0] = dir_frame(0,i);
		direction[1] = dir_frame(1,i);
		direction[2] = dir_frame(2,i);

		if (direction.length() > draw_threshold) {
			arrow_entities[i]->setEnabled(true);
			float scale = direction.length() * arrow_scale_factor;
			direction.normalize();
			arrow_transforms[i]->setScale(scale);
			arrow_transforms[i]->setTranslation(trans);
			arrow_transforms[i]->setRotation(QQuaternion::fromDirection(direction, QVector3D(0,1,0)));
		} else {
			arrow_entities[i]->setEnabled(false);
		}
	}
}

QEntity* ArrowFieldModelExtension::getVisual() {
	if (arrow_positions.size() == 0 || arrow_count == 0)
		return nullptr;

	for (int i=0;i<arrow_count;i++) {
		QEntity* arrow_entity = new QEntity(arrow_field_root);

		Qt3DExtras::QPhongAlphaMaterial* material = new QPhongAlphaMaterial;
		material->setAmbient(arrow_color);
		float alpha = (float)arrow_color.alpha() / 255.;
		material->setAlpha(alpha);

		Qt3DCore::QTransform* transform = new Qt3DCore::QTransform;

		arrow_entity->addComponent(arrow_mesh);
		arrow_entity->addComponent(material);
		arrow_entity->addComponent(transform);

		arrow_transforms.push_back(std::move(transform));
		arrow_entities.push_back(std::move(arrow_entity));
	}

	update(0.);

	return arrow_field_root;
}


void ArrowFieldModelExtension::addArrowFieldFrame(float time, const Matrix3fd& pos, const Matrix3fd& dir) {
	if (arrow_count == 0) {
		arrow_count = pos.size() / 3;
	}

	if (pos.size() != dir.size()) {
		throw RBDLToolkitError("Mismatch in amount of positions and directions provided!");
	}
	if (pos.size()/3 != arrow_count) {
		throw RBDLToolkitError("Amount of arrows does not match the amount previously provided!");
	}

	arrow_times.push_back(std::move(time));
	arrow_positions.push_back(std::move(pos));
	arrow_directions.push_back(std::move(dir));
}
