#ifndef rbdl_wrapper_h_INCLUDED
#define rbdl_wrapper_h_INCLUDED

#include <rbdl/rbdl.h>
#include <rbdl/rbdl_math.h>
#include <rbdl/addons/luamodel/luamodel.h>
#include <rbdl/addons/luamodel/luatables.h>

#include <QString>
#include <Qt3DCore/QEntity>


class RBDLModelWrapper {
	private:
		QString model_file;

		LuaTable model_luatable;
		Matrix3_t axis_transform;
		RigidBodyDynamics::Model* rbdl_model;

		Qt3DCore::QEntity* model_render_obj;
		std::map<std::string, Qt3DCore::QEntity*> body_mesh_map;


	public:
		RBDLModelWrapper();

		Qt3DCore::QEntity* getRenderObj() { return model_render_obj; }

		Qt3DCore::QEntity* loadFromFile(QString model_file);

		void reload();

};

#endif 

