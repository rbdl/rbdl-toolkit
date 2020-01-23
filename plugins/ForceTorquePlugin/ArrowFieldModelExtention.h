#ifndef ARROWFIELDMODELEXTENTION_H_INCLUDED
#define ARROWFIELDMODELEXTENTION_H_INCLUDED

#include <rbdl_wrapper.h>
#include <Qt3DRender>
#include <Qt3DCore>
#include <vector>

#include <Eigen/Dense>

typedef Eigen::Matrix<float, 3, Eigen::Dynamic> Matrix3fd;

class ArrowFieldModelExtention : public WrapperExtention {
	private:
		float max_time; 
		int arrow_count;
		QString group_name;
		QColor arrow_color;

		Qt3DCore::QEntity* arrow_field_root;
		Qt3DRender::QMesh* arrow_mesh;
		
		std::vector<float> arrow_times;
		std::vector<Matrix3fd> arrow_positions;
		std::vector<Matrix3fd> arrow_directions;

		std::vector<Qt3DCore::QTransform*> arrow_transforms;

	public:
		ArrowFieldModelExtention(Qt3DRender::QMesh* arrow_mesh, QString group_name, QColor arrow_color);

		std::string getExtentionName();
		void update(float current_time);
		Qt3DCore::QEntity* getVisual();

		void addArrowFieldFrame(float time, const Matrix3fd& positions, const Matrix3fd& directions);
};

#endif

