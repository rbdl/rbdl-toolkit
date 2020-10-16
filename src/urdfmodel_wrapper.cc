#include <fstream>

#include "urdfmodel_wrapper.h"

UrdfModelWrapper::UrdfModelWrapper() : RBDLModelWrapper() {
	model_type = MODELTYPE_URDF;
};

void UrdfModelWrapper::load(QString model_file) {
	this->model_file = model_file;
	auto model_file_info = QFileInfo(model_file);

	//loading model into rbdl to check its validity, may throw error
	rbdl_model = new RigidBodyDynamics::Model();
	// use try catch after fixing rbdl addon urdf to use exceptions
	//try {
	//} catch (Errors::RBDLError& err) {
	//	// send error up the stack 
	//	throw err;
	//}
	std::ifstream model_file_stream(model_file.toStdString().c_str());
	model_file_stream.seekg(0, std::ios::end);
	model_xml_string.reserve(model_file_stream.tellg());
	model_file_stream.seekg(0, std::ios::beg);
	model_xml_string.assign((std::istreambuf_iterator<char>(model_file_stream)),
	                         std::istreambuf_iterator<char>());
	model_file_stream.close();

	if (!RigidBodyDynamics::Addons::URDFReadFromString(model_xml_string.c_str(), rbdl_model, false)) {
		throw RigidBodyDynamics::Errors::RBDLFileParseError("Error reading urdf file!");
	}


	//load relevant information from modelfile
	auto model_info = loadModelInfo();
	auto segments_info = loadSegmentInfo();

	//construct model from that info
	build3DEntity(model_info, segments_info);
}

ModelInfo UrdfModelWrapper::loadModelInfo() {
	ModelInfo info = { 
	                   QMatrix4x4(),
	                   1.
	                 };
	return info;
}

std::vector<SegmentVisualInfo> UrdfModelWrapper::loadSegmentInfo() {
	std::vector<SegmentVisualInfo> info;

	//auto urdf_model = urdf::praseURDF(model_xml_string);

	return info;
}
