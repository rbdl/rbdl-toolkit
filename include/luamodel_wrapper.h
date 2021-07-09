#ifndef LUAMODEL_WRAPPER_H_INCLUDED
#define LUAMODEL_WRAPPER_H_INCLUDED

#include <rbdl/rbdl.h>
#include <rbdl/rbdl_math.h>
#include <rbdl/addons/luamodel/luamodel.h>
#include <rbdl/addons/luamodel/luatables.h>

#include "rbdl_wrapper.h"

#define MODELTYPE_LUA "lua_model"

class TOOLKIT_DLLAPI LuaModelWrapper : public RBDLModelWrapper {
	public:
		LuaModelWrapper();
		LuaTable model_luatable;

		void load(QString model_file);
		ModelInfo loadModelInfo();
		std::vector<SegmentVisualInfo> loadSegmentInfo();
};

#endif


