#include <iostream>
#include <clocale>

#include <QFileDialog>
#include <QMessageBox>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QProgressDialog>

#include <toolkit_errors.h>
#include <rbdl_wrapper.h>

#include <InverseKinematicsPlugin.h>

using namespace RigidBodyDynamics;
using namespace RigidBodyDynamics::Math;

InverseKinematicsPlugin::InverseKinematicsPlugin() {
	parentApp = NULL;
}

InverseKinematicsPlugin::~InverseKinematicsPlugin() {
}

void InverseKinematicsPlugin::init(ToolkitApp* app) {
	//save reference to parent ToolkitApp 
	parentApp = app;

	compute_menu = app->getMenu("Compute");
	calculate_animation.setText("Inverse Kinematics");
	connect(&calculate_animation, &QAction::triggered, this, &InverseKinematicsPlugin::run_ik);

	compute_menu->addAction(&calculate_animation);
}

bool only_models_with_markers_and_mocap_data(RBDLModelWrapper* model) {
	if (model->hasExtention("ModelMarker") && model->hasExtention("MocapMarker")) {
		return true;
	}
	return false;
}

std::vector<unsigned> get_missing_indexes(unsigned size, const std::vector<unsigned>& indexes) {
	std::vector<unsigned> missing;

	for (int i=0;i<size;i++) {
		bool found = false;
		for (auto index : indexes) {
			if (i == index) {
				found = true;
				break;
			}
		}
		if (!found) {
			missing.push_back(i);
		}
	}

	return missing;
}

void InverseKinematicsPlugin::run_ik() {
	RBDLModelWrapper* model = parentApp->selectModel(only_models_with_markers_and_mocap_data);

	if(!only_models_with_markers_and_mocap_data(model)) {
		auto err = RBDLToolkitError("Model is Missing Mocap/Model Markers, so running IK is not possible due to missing data!");
		ToolkitApp::showExceptionDialog(err);
		return;
	}

	auto markers_ext = (ModelMarkerExtention*)model->getExtention("ModelMarker");
	auto mocap_ext = (MotionMarkerExtention*)model->getExtention("MocapMarker");

	//mapping markers
	std::map<unsigned, unsigned> index_mapping;
	std::vector<unsigned> used_model_markers;
	std::vector<unsigned> used_mocap_markers;

	for (int i=0; i<markers_ext->getMarkerLabels().size(); i++) {
		for(int j=0; j<mocap_ext->getMarkerLabels().size(); j++) {
			if (markers_ext->getMarkerLabels()[i] == mocap_ext->getMarkerLabels()[j]) {
				index_mapping[i] == j;
				used_model_markers.push_back(i);
				used_mocap_markers.push_back(j);
			}
		}
	}

	//Display Warning when unused markers are detected
	if(! ( (used_mocap_markers.size() == mocap_ext->getMarkerLabels().size()) && (used_model_markers.size() == markers_ext->getMarkerLabels().size()) )) {
		QString warning_msg("Warning! Some Markes do not match any marker in the Model/Mocap data! Ignoring these markers during fit!\n\n");

		auto missing_indexes = get_missing_indexes(markers_ext->getMarkerLabels().size(), used_model_markers);
		if (missing_indexes.size() > 0) {
			warning_msg.append(QString("Ignored Model Markers (%1):\n").arg(missing_indexes.size()));
			for ( auto missing_index : missing_indexes ) {
				warning_msg.append(QString("%1\n").arg(QString(markers_ext->getMarkerLabels()[missing_index].c_str())));
			}
		}

		missing_indexes = get_missing_indexes(mocap_ext->getMarkerLabels().size(), used_mocap_markers);
		if (missing_indexes.size() > 0) {
			warning_msg.append(QString("Ignored Mocap Markers (%1):\n").arg(missing_indexes.size()));
			for ( auto missing_index : missing_indexes) {
				warning_msg.append(QString("%1\n").arg(QString(mocap_ext->getMarkerLabels()[missing_index].c_str())));
			}
		}
		parentApp->showWarningDialog(warning_msg);
	}

	//setup body marker data
	std::vector<unsigned int> body_ids;
	std::vector<Vector3d> body_marker_pos;
	for (auto i : used_model_markers) {
		auto segment_name = markers_ext->getMarkerSegments()[i];
		auto id = model->rbdl_model->GetBodyId(segment_name.c_str());
		if (id != std::numeric_limits<unsigned int>::max() ) {
			body_ids.push_back(id);
			body_marker_pos.push_back(markers_ext->getMarkerPositions()[i]);
		} else {
			//should never happen, since all markes are defined on a segment in the luamodel
			auto err = RBDLToolkitError("Found aktive marker while performing IK, that is not mapped to a segment! Aborting …");
			ToolkitApp::showExceptionDialog(err);
			return;
		}
	}

	//IK
	AnimationModelExtention* ext = new AnimationModelExtention();

	VectorNd Qinit = VectorNd::Zero(model->rbdl_model->dof_count);
	VectorNd Qres = VectorNd::Zero(model->rbdl_model->dof_count);
	auto times = mocap_ext->getMarkerTimes();

	QProgressDialog pbar("Calculating Model Animation with IK", "Abort Computation", 0, times.size(), nullptr);
	pbar.setWindowModality(Qt::WindowModal);
	pbar.setMinimumDuration(0);

	for ( int frame_i=0; frame_i<times.size(); frame_i++ ) {
		//find all target positions of the marker in the mocap data
		pbar.setValue(frame_i);
		std::vector<Vector3d> target_pos;
		auto mocap_frame = mocap_ext->getMarkerFrames()[frame_i];
		for (auto i : used_model_markers) {
			auto mocap_mapped_id = index_mapping[i]; 
			target_pos.push_back(std::move(
			       Vector3d(mocap_frame(0, mocap_mapped_id),
			                mocap_frame(1, mocap_mapped_id),
			                mocap_frame(2, mocap_mapped_id)
			               )
			       )
			);
		}
		if (!InverseKinematics(*(model->rbdl_model), Qinit, body_ids, body_marker_pos, target_pos, Qres)) {
			std::cout << "Fit has bigger error than wanted!" << std::endl;
		}
		std::cout << Qres << std::endl;
		ext->addAnimationFrame(times[frame_i], Qres);
		Qinit = Qres;
	}

	model->addExtention(ext);
}

