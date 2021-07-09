#ifndef rbdl_wrapper_h_INCLUDED
#define rbdl_wrapper_h_INCLUDED

#include <rbdl/rbdl.h>
#include <rbdl/rbdl_math.h>
#include <rbdl/addons/luamodel/luamodel.h>
#include <rbdl/addons/luamodel/luatables.h>

#include <QColor>
#include <QString>
#include <QFileInfo>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include "toolkit_config.h"

class RBDLModelWrapper;


/* This class provides an abstact base for adding data to a loaded model, such as (Animations, Forces, etc.).
 */
class WrapperExtension {
	protected:
		RBDLModelWrapper* model_parent;

	public:
		WrapperExtension();

		void setModelParent(RBDLModelWrapper* model);

		//needs to be implemented by every extension
		virtual std::string getExtensionName() = 0;
		virtual void update(float current_time) = 0; 

		//optional implementetion, default does nothing
		virtual Qt3DCore::QEntity* getVisual();
};

struct ModelInfo {
	QMatrix4x4 model_world_transform;
	float unit_scaling;
};

struct SegmentVisualInfo {
	std::string segment_name;

	QString mesh_file;
	QVector3D mesh_translation;
	QQuaternion mesh_rotation;

	QColor visual_color;
	QVector3D visual_center;
	QVector3D visual_dimensions;
};


/* This class provides a wrapper around rbdl models, in order to visualize them with Qt3D
 */
class TOOLKIT_DLLAPI RBDLModelWrapper : public QObject {
	Q_OBJECT
	protected:
		QString model_file;
		std::string model_type;

		Qt3DCore::QEntity* model_root;
		std::map<std::string, Qt3DCore::QEntity*> body_segment_map;
		std::map<std::string, Qt3DCore::QTransform*> body_transform_map;
		float mesh_unit_scaling;

		//all loaded extra data is supposed to be loaded as an extension to the model
		std::vector<std::string> extension_names;
		std::map<std::string, WrapperExtension*> extensions;

		RigidBodyDynamics::Math::VectorNd current_Q;

		void clear();
		virtual void load(QString model_file) = 0;
	public:
		TOOLKIT_DLLAPI static RBDLModelWrapper* loadFromFile(QString model_file);


		RigidBodyDynamics::Model* rbdl_model;

		RBDLModelWrapper();
		TOOLKIT_DLLAPI void build3DEntity(ModelInfo&, std::vector<SegmentVisualInfo>&);

		TOOLKIT_DLLAPI QString getFileName() { return QFileInfo(model_file).baseName(); }
		TOOLKIT_DLLAPI std::string getModelType() { return model_type; }
		TOOLKIT_DLLAPI QString getModelFile();
		TOOLKIT_DLLAPI int getModelDof();
		TOOLKIT_DLLAPI Qt3DCore::QEntity* getRenderObj() { return model_root; }
		TOOLKIT_DLLAPI Qt3DCore::QEntity* getSegmentEntity(std::string segment_name, bool create=false);

		TOOLKIT_DLLAPI void addStaticVisual(std::string segment_name, Qt3DCore::QEntity *visual);
		TOOLKIT_DLLAPI void updateKinematics(RigidBodyDynamics::Math::VectorNd Q);

		TOOLKIT_DLLAPI void reload();

		//takes ownership of extension -> only delete via model not where it was created
		TOOLKIT_DLLAPI void addExtension(WrapperExtension* extension);
		//void deleteExtension(std::string name);
		TOOLKIT_DLLAPI bool hasExtension(std::string name);
		TOOLKIT_DLLAPI const std::vector<std::string>& loadedExtensions() { return extension_names; }
		TOOLKIT_DLLAPI WrapperExtension* getExtension(std::string name);

	public slots:
		void model_update(float current_time);

	signals:
		void new_extension_added();
		void visual_added(Qt3DCore::QEntity* visual);

};


#endif 

