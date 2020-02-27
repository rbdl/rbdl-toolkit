#ifndef ARROWFIELDMODELEXTENTION_H_INCLUDED
#define ARROWFIELDMODELEXTENTION_H_INCLUDED

#include <rbdl_wrapper.h>
#include <Qt3DRender>
#include <Qt3DCore>
#include <vector>

#include <Eigen/Dense>

typedef Eigen::Matrix<float, 3, Eigen::Dynamic> Matrix3fd;

class MotionMarkerExtention : public WrapperExtention {
	private:
		float max_time; 
		int marker_count;

		QString group_name;

		std::vector<float> marker_times;
		std::vector<Matrix3fd> marker_positions;

		std::vector<Qt3DCore::QTransform*> marker_transforms;
		std::vector<Qt3DCore::QEntity*> marker_entities;

	public:
		MotionMarkerExtention(QString group_name);

		std::string getExtentionName();
		void update(float current_time);
		Qt3DCore::QEntity* getVisual();

};

#endif

