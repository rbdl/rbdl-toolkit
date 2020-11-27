#include <fstream>
#include <QFileInfo>
#include <QDir>

#include "urdfmodel_wrapper.h"
#include "urdf_parser/urdf_parser.h"

#include "util.h"

UrdfModelWrapper::UrdfModelWrapper() : RBDLModelWrapper() {
	model_type = MODELTYPE_URDF;
};

void UrdfModelWrapper::load(QString model_file) {
	this->model_file = model_file;
	auto model_file_info = QFileInfo(model_file);

	auto last_pwd = QDir::currentPath();
	auto model_pwd = model_file_info.absoluteDir().absolutePath();
	QDir::setCurrent(model_pwd);

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

	auto urdf_model = urdf::parseURDF(model_xml_string);

	//if (!RigidBodyDynamics::Addons::URDFReadFromString(model_xml_string.c_str(), rbdl_model, false)) {
	//	throw RigidBodyDynamics::Errors::RBDLFileParseError("Error reading urdf file!");
	//}


	//load relevant information from modelfile
	auto model_info = loadModelInfo();
	auto segments_info = loadSegmentInfo();

	//construct model from that info
	build3DEntity(model_info, segments_info);

	//return to original pwd
	QDir::setCurrent(last_pwd);
}

ModelInfo UrdfModelWrapper::loadModelInfo() {
	QMatrix4x4 orientation;
	orientation.rotate(-90., 1., 0., 0.);
	ModelInfo info = { 
	                   orientation,
	                   1.
	                 };
	return info;
}

// this function tries to merge paths based on common point
QString mergePaths(QString upper, QString lower) {
	int i=1;
	while (i < upper.size() && i < lower.size()) {
		if (upper.endsWith(lower.left(i), Qt::CaseInsensitive)) {
			QString merge = QString(upper).append(lower.right(lower.size()-i));
			auto info = QFileInfo(merge);
			if (info.exists()) {
				return merge;
			}
		}
		i++;
	}
	throw std::exception();
}

void findRelativeToRosPackageRoot(QString& path, QString model_file_path) {
	QString backup_orig = QString(path);
	path.replace("package://", "");
	auto package_path = QFileInfo(model_file_path).absoluteDir();

	// if we now find the file great, stop here
	if (package_path.exists(path)) {
		path = package_path.absoluteFilePath(path);
		return;
	}

	// try to find the package root, by looking for the package xml
	while ( !package_path.exists("package.xml") && !package_path.isRoot() ) {
		package_path.cdUp();
	}

	// if we now find the file great, stop here
	if (package_path.exists(path)) {
		path = package_path.absoluteFilePath(path);
		return;
	}
	// we did not find the file

	// if we went to the root go back where we started from
	// else we have found the Ros Package root and should look from there
	if (package_path.isRoot()) {
		package_path = QFileInfo(model_file_path).absoluteDir();
	}

	try {
		path = mergePaths(package_path.canonicalPath(), path);
	} catch (std::exception &e){
		std::ostringstream errormsg;
		errormsg << "No location found to load resource: " << backup_orig.toStdString() << " from! This model may be broken." << std::endl;
		throw RigidBodyDynamics::Errors::RBDLFileParseError(errormsg.str());
	}
}

std::vector<SegmentVisualInfo> UrdfModelWrapper::loadSegmentInfo() {
	std::vector<SegmentVisualInfo> info;

	auto urdf_model = urdf::parseURDF(model_xml_string);

	for ( auto link = urdf_model->links_.begin(); link != urdf_model->links_.end(); link++ ) {
		auto l = link->second.get();
		while ( rbdl_model->GetBodyId(l->name.c_str()) > rbdl_model->dof_count ) {
			if (l->getParent() == nullptr ) {
				break;
			} else {
				l = l->getParent();
			}
		}
		bool add_to_root = rbdl_model->GetBodyId(l->name.c_str()) > rbdl_model->dof_count;
		auto segment_name = l->name;

		l = link->second.get();

		if ( l->visual != nullptr ) {
			auto visual = l->visual.get();

			auto pos = visual->origin.position;
			//QVector3D visual_center = QVector3D(pos.x, pos.y, pos.z);
			QVector3D visual_center = QVector3D(0, 0, 0);
			QVector3D mesh_translation = QVector3D(0, 0, 0);

			auto rot = visual->origin.rotation;
			QQuaternion mesh_rotation = QQuaternion(rot.x, rot.y, rot.z, rot.w);


			QColor visual_color = QColor::fromRgbF(1., 1., 1., 1.);
			if (visual->material != nullptr) {
				auto color = visual->material->color;
				visual_color = QColor::fromRgbF(color.r, color.g, color.b, color.a);
			}

			QString mesh_file;
			QVector3D visual_dimentions;

			switch (visual->geometry->type) {
				case urdf::Geometry::SPHERE: {
					urdf::Sphere* geo = (urdf::Sphere*)visual->geometry.get();
					mesh_file = findFile(QString("unit_sphere_medres.obj"),true);
					visual_dimentions = QVector3D(geo->radius, geo->radius, geo->radius);
					break;
				}
				case urdf::Geometry::BOX: {
					urdf::Box* geo = (urdf::Box*)visual->geometry.get();
					mesh_file = findFile(QString("unit_cube.obj"), true);
					visual_dimentions = QVector3D(geo->dim.x, geo->dim.y, geo->dim.z);
					break;
				}
				case urdf::Geometry::CYLINDER: {
					urdf::Cylinder* geo = (urdf::Cylinder*)visual->geometry.get();
					mesh_file = findFile(QString("unit_cylinder_medres_z.obj"),true);
					visual_dimentions = QVector3D(geo->radius, geo->radius, geo->length);
					break;
				}
				case urdf::Geometry::MESH :{
					urdf::Mesh* geo = (urdf::Mesh*)visual->geometry.get();
					QString filename = QString::fromStdString(geo->filename);
					if (filename.startsWith("package://")) {
						findRelativeToRosPackageRoot(filename, this->model_file);
					}
					mesh_file = findFile(filename, true);
					visual_dimentions = QVector3D(geo->scale.x, geo->scale.y, geo->scale.z);
					break;
				}
			}
			std::cout << mesh_file.toStdString() << std::endl;
			QString msg;
			QDebug(&msg) << visual_dimentions;
			std::cout << msg.toStdString() << std::endl;
			SegmentVisualInfo si = {
			    segment_name,
			    mesh_file,
			    mesh_translation,
			    mesh_rotation,
			    visual_color,
			    visual_center,
			    visual_dimentions
			};
			info.push_back(std::move(si));
		}
	}

	return info;
}
