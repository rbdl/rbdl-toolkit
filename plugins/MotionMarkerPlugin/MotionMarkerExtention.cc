#include "MotionMarkerExtention.h"
#include "toolkit_errors.h"

#include <iostream>

#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

MotionMarkerExtention::MotionMarkerExtention(
	QString group_name) : 
		group_name(group_name),
		WrapperExtention()
{
	//arrow_field_root = new QEntity;
	//arrow_field_root->setProperty("Scene.ObjGroup", QVariant(group_name));
}

std::string MotionMarkerExtention::getExtentionName() {
	return group_name.toStdString();
}

void MotionMarkerExtention::update(float current_time) {
	//if (arrow_positions.size() == 0 || arrow_count == 0)
	//	return;

	//unsigned int time_index = 0;
	//for (; time_index < arrow_times.size(); time_index++) {
	//	if (time_index == arrow_times.size()-1) break;
	//	if (arrow_times[time_index] <= current_time && arrow_times[time_index+1] > current_time) {
	//		break;
	//	}
	//}
}

QEntity* MotionMarkerExtention::getVisual() {
	return nullptr;
}

