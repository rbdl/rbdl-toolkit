#ifndef ARROWFIELDMODELEXTENTION_H_INCLUDED
#define ARROWFIELDMODELEXTENTION_H_INCLUDED

#include <rbdl_wrapper.h>
#include <Qt3DRender>
#include <Qt3DCore>
#include <vector>

#include <Eigen/Dense>

typedef Eigen::Matrix<float, 3, Eigen::Dynamic> Matrix3fd;

class ArrowFieldModelExtension : public WrapperExtension {
	private:
		float max_time; 
		int arrow_count;

		float draw_threshold;
		float arrow_scale_factor;

		QString group_name;
		QColor arrow_color;

		Qt3DCore::QEntity* arrow_field_root;
		Qt3DRender::QMesh* arrow_mesh;
		
		std::vector<float> arrow_times;
		std::vector<Matrix3fd> arrow_positions;
		std::vector<Matrix3fd> arrow_directions;

		std::vector<Qt3DCore::QTransform*> arrow_transforms;
		std::vector<Qt3DCore::QEntity*> arrow_entities;

	public:
		ArrowFieldModelExtension(QString arrow_mesh_path, 
		                         QString group_name,
		                         QColor arrow_color,
		                         float draw_threshold = 0.001,
		                         float arrow_scale_factor = 0.001);

		std::string getExtensionName();
		void update(float current_time);
		Qt3DCore::QEntity* getVisual();

		void addArrowFieldFrame(float time, const Matrix3fd& positions, const Matrix3fd& directions);

		void setDrawTheshold(float draw_threshold) { this->draw_threshold = draw_threshold; }
		void setScaleFactor(float arrow_scale_factor) { this->arrow_scale_factor = arrow_scale_factor; }
};

#endif

