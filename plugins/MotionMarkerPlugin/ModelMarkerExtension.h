#ifndef MODELMARKEREXTENTION_H_INCLUDED
#define MODELMARKEREXTENTION_H_INCLUDED

#include <rbdl_wrapper.h>
#include <Qt3DRender>
#include <Qt3DCore>
#include <vector>

#include <Eigen/Dense>

#include "toolkit_config.h"

using namespace RigidBodyDynamics::Math;

typedef Eigen::Matrix<float, 3, Eigen::Dynamic> Matrix3fd;

class TOOLKIT_PLUGIN_DLLAPI ModelMarkerExtension : public WrapperExtension {
	private:
		float marker_size; 
		QColor marker_color;

		Qt3DRender::QMesh* marker_mesh;

		std::vector<std::string> marker_names;
		std::vector<std::string> marker_segments;
		std::vector<Vector3d> marker_positions;

		std::vector<Qt3DCore::QEntity*> marker_visuals;
	public:
		ModelMarkerExtension(QColor marker_color, float marker_size);

		void addModelMarker(const std::string& label, const std::string& segment_name, const Vector3d& position);

		std::string getExtensionName();
		void update(float current_time);
		Qt3DCore::QEntity* getVisual();

		//Data Methods to access marker data
		const std::vector<std::string>& getMarkerLabels() { return marker_names; }
		const std::vector<std::string>& getMarkerSegments() { return marker_segments; }
		const std::vector<Vector3d>& getMarkerPositions() { return marker_positions; }
};

#endif

