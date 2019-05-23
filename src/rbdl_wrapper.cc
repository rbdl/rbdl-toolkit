#include "rbdl_wrapper.h"

#include <QFileInfo>

#include "render_util.h"

RBDLModelWrapper::RBDLModelWrapper(){
	model_render_obj = NULL;
}

Qt3DCore::QEntity* RBDLModelWrapper::loadFromFile(QString model_file) {

	QFileInfo check_file(model_file);
	// Is it really a file and no directory?
	if (!check_file.exists() || !check_file.isFile()) {
		//Todo raise error
	}

	this->model_file = model_file;

	//try loading model into rbdl to check its validity
	if (!RigidBodyDynamics::Addons::LuaModelReadFromFile(model_file.toStdString().c_str(), &rbdl_model, false)) {
		//Todo raise error
	}

	// load model lua extra to read parameters for rendering
	model_luatable = LuaTable::fromFile(model_file.toStdString().c_str());

	unsigned int segments_cnt = model_luatable["frames"].length();

	for (int i=1; i<=segments_cnt ; i++) {
		std::string segment_name = model_luatable["frames"][i]["name"].get<std::string>();
	}

	// create renderable scenegraph obj 
	model_render_obj = new Qt3DCore::QEntity();


	return model_render_obj;
}
