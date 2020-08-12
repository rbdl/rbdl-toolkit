#ifndef rbdl_wrapper_h_INCLUDED
#define rbdl_wrapper_h_INCLUDED

#include <rbdl/rbdl.h>
#include <rbdl/rbdl_math.h>
#include <rbdl/addons/luamodel/luamodel.h>
#include <rbdl/addons/luamodel/luatables.h>

#include <QString>
#include <QFileInfo>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

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


/* This class provides a wrapper around rbdl models, in order to visualize them with Qt3D
 */
class RBDLModelWrapper : public QObject {
	Q_OBJECT
	private:
		QString model_file;

		Qt3DCore::QEntity* model_render_obj;
		std::map<std::string, Qt3DCore::QEntity*> body_mesh_map;
		std::map<std::string, Qt3DCore::QTransform*> body_transform_map;
		float mesh_unit_scaling;

		//all loaded extra data is supposed to be loaded as an extension to the model
		std::vector<std::string> extension_names;
		std::map<std::string, WrapperExtension*> extensions;
	public:
		LuaTable model_luatable;

		RigidBodyDynamics::Model* rbdl_model;

		RBDLModelWrapper();

		Qt3DCore::QEntity* getRenderObj() { return model_render_obj; }

		Qt3DCore::QEntity* loadFromFile(QString model_file);

		//takes ownership of extension -> only delete via model not where it was created
		void addExtension(WrapperExtension* extension);
		void addStaticVisual(std::string segment_name, Qt3DCore::QEntity *visual);
		//void deleteExtension(std::string name);
		bool hasExtension(std::string name);
		const std::vector<std::string>& loadedExtensions() { return extension_names; }
		WrapperExtension* getExtension(std::string name);

		void updateKinematics(RigidBodyDynamics::Math::VectorNd Q);
		QString getFileName() { return QFileInfo(model_file).baseName(); }

		int getModelDof();

		void reload();

		QString getModelFile();

	public slots:
		void model_update(float current_time);

	signals:
		void new_extension_added();
		void visual_added(Qt3DCore::QEntity* visual);

};

#endif 

