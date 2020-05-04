#ifndef MOTIONMARKEREXTENTION_H_INCLUDED
#define MOTIONMARKEREXTENTION_H_INCLUDED

#include <rbdl_wrapper.h>
#include <Qt3DRender>
#include <Qt3DCore>
#include <vector>

#include <Eigen/Dense>

typedef Eigen::Matrix<float, 3, Eigen::Dynamic> Matrix3fd;

class MotionMarkerExtention : public WrapperExtention {
	private:
		float max_time; 
		unsigned int marker_count;
		float marker_size; 
		QColor marker_color;

		Qt3DCore::QEntity* marker_plain_root;
		Qt3DRender::QMesh marker_mesh;

		std::vector<float> marker_times;
		std::vector<Matrix3fd> marker_positions;
		std::vector<std::string> marker_names;

		std::vector<Qt3DCore::QTransform*> marker_transforms;
		std::vector<Qt3DCore::QEntity*> marker_entities;
	public:
		MotionMarkerExtention(unsigned int marker_count, QColor marker_color, float marker_size);
		float getMaxTime() { return max_time; }

		void addMocapMarkerFrame(float time, const Matrix3fd& marker_pos);

		void setMarkerLabel(unsigned int marker_data_pos, std::string& label);

		std::string getExtentionName();
		void update(float current_time);
		Qt3DCore::QEntity* getVisual();

		//Data Methods to access marker data
		const std::vector<std::string>& getMarkerLabels() { return marker_names; }
		const std::vector<Matrix3fd>& getMarkerFrames() { return marker_positions; }
};

#endif

