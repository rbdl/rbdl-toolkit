#include <QFileInfo>
#include <QDir>

#include "luamodel_wrapper.h"
#include "util.h"

using namespace RigidBodyDynamics;
using namespace RigidBodyDynamics::Math;

LuaModelWrapper::LuaModelWrapper() : RBDLModelWrapper() {
	model_type = MODELTYPE_LUA;
}

void LuaModelWrapper::load(QString model_file) {
	auto model_file_info = QFileInfo(model_file);
	this->model_file = model_file_info.absoluteFilePath();
	rbdl_model = new RigidBodyDynamics::Model();

	// some logic to save current pwd and move to the dir where the model is loaded from
	// needed so that lua model scripts with relative dependencies work correctly!
	auto last_pwd = QDir::currentPath();
	auto model_pwd = model_file_info.absoluteDir().absolutePath();
	QDir::setCurrent(model_pwd);


	//loading model into rbdl to check its validity, may throw error
	try {
		RigidBodyDynamics::Addons::LuaModelReadFromFile(model_file.toStdString().c_str(), rbdl_model, false);
		//load model lua extra to read parameters for rendering
		model_luatable = LuaTable::fromFile(model_file.toStdString().c_str());
		//std::cout << model_luatable.serialize() << std::endl;
	} catch (Errors::RBDLError& err) {
		// return to original pwd
		QDir::setCurrent(last_pwd);
		// send error up the stack 
		throw err;
	}

	//load relevant information from modelfile
	auto model_info = loadModelInfo();
	auto segments_info = loadSegmentInfo();

	//construct model from that info
	build3DEntity(model_info, segments_info);

	//return to original pwd
	QDir::setCurrent(last_pwd);
}

std::vector<SegmentVisualInfo> LuaModelWrapper::loadSegmentInfo() {
	std::vector<SegmentVisualInfo> info;

	unsigned int segments_cnt = model_luatable["frames"].length();
	for (int i=1; i<=segments_cnt ; i++)  {

		auto segment_name = model_luatable["frames"][i]["name"].get<std::string>();

		//every segment can render multiple visuals
		unsigned int visuals_cnt = model_luatable["frames"][i]["visuals"].length();

		for (int j=1; j<=visuals_cnt; j++) {
			float angle;
			Vector3d axis;

			if (model_luatable["frames"][i]["visuals"][j]["rotate"].exists()) {
				angle = model_luatable["frames"][i]["visuals"][j]["rotate"]["angle"].getDefault(0.f);
				axis = model_luatable["frames"][i]["visuals"][j]["rotate"]["axis"].getDefault(Vector3d(1., 0., 0.));
			} else {
				angle = 0.f;
				axis = Vector3d(1., 0., 0.);
			}
			axis = axis.normalized();
			auto mesh_rotation = QQuaternion::fromAxisAndAngle(QVector3D(axis[0], axis[1], axis[2]), angle);

			Vector3d mesh_translate = model_luatable["frames"][i]["visuals"][j]["translate"].getDefault(Vector3d(0., 0., 0.));

			//Mesh src
			std::string src = model_luatable["frames"][i]["visuals"][j]["src"].get<std::string>();
			QString mesh_src = findFile(src, true);

			//Wrapper Entity 
			Vector3d visual_scale = model_luatable["frames"][i]["visuals"][j]["scale"].getDefault(Vector3d(1., 1., 1.));
			Vector3d visual_dimensions = model_luatable["frames"][i]["visuals"][j]["dimensions"].getDefault(Vector3d(1., 1., 1.));
			
			Vector3d visual_center = model_luatable["frames"][i]["visuals"][j]["mesh_center"].getDefault(Vector3d(0., 0., 0.));
			
			Vector3d visual_color_rgb = model_luatable["frames"][i]["visuals"][j]["color"].getDefault(Vector3d(1., 1., 1.));
			float visual_alpha = model_luatable["frames"][i]["visuals"][j]["alpha"].getDefault(1.);
			QColor visual_color = QColor::fromRgbF(visual_color_rgb[0], visual_color_rgb[1], visual_color_rgb[2], visual_alpha);

			SegmentVisualInfo si = {
			                           segment_name,
			                           mesh_src,
			                           to_qt_vector(mesh_translate),
			                           mesh_rotation,
			                           visual_color,
			                           to_qt_vector(visual_center),
			                           QVector3D(visual_dimensions[0] * visual_scale[0], visual_dimensions[1] * visual_scale[1], visual_dimensions[2] * visual_scale[2])
			                       };
			info.push_back(std::move(si));
		}
	}
	return info;
}

ModelInfo LuaModelWrapper::loadModelInfo() {
	ModelInfo info = {};

	//read axes form model file
	Vector3d axis_front = model_luatable["configuration"]["axis_front"].getDefault(Vector3d(1., 0., 0.)); 
	Vector3d axis_up = model_luatable["configuration"]["axis_up"].getDefault(Vector3d(0., 1., 0.));
	Vector3d axis_right = model_luatable["configuration"]["axis_right"].getDefault(Vector3d(0., 0., 1.));

	info.model_world_transform = QMatrix4x4( axis_right[0], axis_right[1], axis_right[2], 0.,
	                                         axis_up[0],    axis_up[1],    axis_up[2],    0.,
	                                         axis_front[0], axis_front[1], axis_front[2], 0.,
	                                         0.,            0.,            0.,            1.
	                                       );

	//measurement unit used by meshes
	std::string mesh_unit = model_luatable["configuration"]["mesh_measurement_unit"].getDefault(std::string("m"));
	if (mesh_unit == "cm") {
			info.unit_scaling = 0.01;
	} else if (mesh_unit == "mm") {
			info.unit_scaling = 0.001;
	// assume meters
	} else {
		info.unit_scaling = 1.;
	}

	return info;
}
