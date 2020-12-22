#include "MotionMarkerPlugin.h"
#include <iostream>
#include <clocale>

#include <QFileDialog>
#include <QMessageBox>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <Qt3DExtras>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QEntity>

#include <parser.hpp>
#include <toolkit_errors.h>
#include <rbdl_wrapper.h>

#include <c3dfile.h>

using namespace RigidBodyDynamics::Math;

MotionMarkerPlugin::MotionMarkerPlugin() {
	parentApp = NULL;
}

MotionMarkerPlugin::~MotionMarkerPlugin() {
}

void MotionMarkerPlugin::init(ToolkitApp* app) {
	//save reference to parent ToolkitApp 
	parentApp = app;

	load_file_trigger = new QAction("Load Motion Markers");
	parentApp->addFileAction(load_file_trigger);

	connect(load_file_trigger, SIGNAL(triggered(bool)), this, SLOT(action_load_data()));

	QCommandLineOption motion_marker_option( QStringList() << "mm" << "motionmarker",
	                                 "Load Motion Marker files <file>", 
	                                 "file"
	                               );
	parentApp->addCmdOption(motion_marker_option, [this](QCommandLineParser& parser){
		auto data_list = parser.values("motionmarker");
		for (int i=0; i<data_list.size(); i++) {
			if (i < parentApp->getLoadedModels()->size() ) {
				MotionMarkerExtension* ext = nullptr; 
				auto file = data_list[i];

				try {
					ext = loadMotionMarkerFile(file);
				} catch (RigidBodyDynamics::Errors::RBDLError& e){
					ToolkitApp::showExceptionDialog(e);
					if (ext != nullptr) 
						delete ext;
				}

				RBDLModelWrapper* rbdl_model = parentApp->getLoadedModels()->at(i);
				rbdl_model->addExtension(ext);
				model_file_map[rbdl_model] = file;
				parentApp->getToolkitTimeline()->setMaxTime(ext->getMaxTime());
			} else {
				std::cout << QString("Mocap file %1 can not be mapped to a model ... Ignoring!").arg(data_list[i]).toStdString() << std::endl;
			}
		}
	});

	loadMarkerSettings();

	//display model markers of all loaded models
	auto all_models = parentApp->getLoadedModels();
	for (auto model : *all_models) {
		addModelMarkersToModel(model);
	}

	connect(parentApp, SIGNAL(model_loaded(RBDLModelWrapper*)), this, SLOT(addModelMarkersToModel(RBDLModelWrapper*)));
	connect(parentApp, &ToolkitApp::reloaded_model, this, &MotionMarkerPlugin::reload);
}

void MotionMarkerPlugin::loadMarkerSettings() {
	parentApp->toolkit_settings.beginGroup("MarkerOptions");

	//marker color
	QVariant val = parentApp->toolkit_settings.value("marker.color");
	if (val.isNull()) {
		marker_color = QColor::fromRgbF(0., 0., 1., 1.);
		parentApp->toolkit_settings.setValue("marker.color", marker_color.rgba());
	} else {
		marker_color = QColor::fromRgba(val.toUInt());
	}
	parentApp->toolkit_settings.setType("marker.color", marker_color);

	//model marker color
	val = parentApp->toolkit_settings.value("marker.color_model");
	if (val.isNull()) {
		marker_color_model = QColor::fromRgbF(1., 1., 1., 1.);
		parentApp->toolkit_settings.setValue("marker.color_model", marker_color_model.rgba());
	} else {
		marker_color_model = QColor::fromRgba(val.toUInt());
	}
	parentApp->toolkit_settings.setType("marker.color_model", marker_color_model);

	//marker size
	val = parentApp->toolkit_settings.value("marker.size");
	if (val.isNull()) {
		marker_size = 0.01;
		parentApp->toolkit_settings.setValue("marker.size", marker_size);
	} else {
		marker_size = val.toFloat();
	}
	parentApp->toolkit_settings.setType("marker.size", marker_size);

	parentApp->toolkit_settings.endGroup();
}

void MotionMarkerPlugin::addModelMarkersToModel(RBDLModelWrapper *rbdl_model) {
	if (rbdl_model->getModelType() == MODELTYPE_LUA) {
		LuaModelWrapper *model = (LuaModelWrapper*)rbdl_model;
		ModelMarkerExtension* ext = new ModelMarkerExtension(marker_color_model, marker_size);
		unsigned int segment_cnt = model->model_luatable["frames"].length();
		for(int i=1; i<=segment_cnt; i++) {
			std::string segment_name = model->model_luatable["frames"][i]["name"].get<std::string>();
			std::vector<LuaKey> keys= model->model_luatable["frames"][i]["markers"].keys();

			for (auto marker_name : keys) {
				Vector3d marker_position = model->model_luatable["frames"][i]["markers"][marker_name.string_value.c_str()].getDefault(Vector3d(0.,0.,0.));
				//marker_position = model->axis_transform * marker_position;

				ext->addModelMarker(marker_name.string_value, segment_name, marker_position);
			}
		}
		if (ext->getMarkerLabels().size() == 0) {
			delete ext;
			return;
		}
		model->addExtension(ext);
	} else {
		parentApp->showWarningDialog("Loading model markes from this model type is not supported.");
	}
}

void MotionMarkerPlugin::action_load_data() {
	if (parentApp != NULL) {
		QFileDialog file_dialog (parentApp, "Select Mocap File");

		file_dialog.setNameFilter(tr("Mocap File (*.c3d)"));
		file_dialog.setFileMode(QFileDialog::ExistingFile);

		MotionMarkerExtension* ext;
		if (file_dialog.exec()) {
			try {
				ext = loadMotionMarkerFile(file_dialog.selectedFiles().at(0));
			} catch (RigidBodyDynamics::Errors::RBDLError& e){
				ToolkitApp::showExceptionDialog(e);
				delete ext;
			}
			if (parentApp->getLoadedModels()->size() != 0) {
				RBDLModelWrapper* rbdl_model = nullptr;

				if (parentApp->getLoadedModels()->size() == 1) {
					rbdl_model = parentApp->getLoadedModels()->at(0);
				} else {
					rbdl_model = parentApp->selectModel(nullptr);
				}

				if (rbdl_model != nullptr) {
					rbdl_model->addExtension(ext);
					model_file_map[rbdl_model] = file_dialog.selectedFiles().at(0);
					parentApp->getToolkitTimeline()->setMaxTime(ext->getMaxTime());
				} else {
					delete ext;
				}
			}
		}	
	} else {
		//should never happen
		throw RigidBodyDynamics::Errors::RBDLError("Marker plugin was not initialized correctly!");
	}
}

MotionMarkerExtension* MotionMarkerPlugin::loadMotionMarkerFile(QString path) {
	C3DFile marker_file;
	if(!marker_file.load(path.toStdString().c_str())) {
		throw RBDLToolkitError("Error loading marker c3d file: " + path.toStdString());
	}

	unsigned int marker_count = marker_file.header.num_markers;
	MotionMarkerExtension* extension = new MotionMarkerExtension(marker_count, marker_color, marker_size);

	float frame_rate = 1. / marker_file.header.video_sampling_rate;
	float current_frame_time = 0.;

	int frame_count = marker_file.header.last_frame - marker_file.header.first_frame;

	for (int i=0;i<frame_count;i++) {
		Matrix3fd pos;
		pos.resize(3, marker_count);

		//loop vars
		bool first_frame_complete = false;
		int j=0; //marker position in matrix

		for (auto marker_label : marker_file.point_label ) {
			FloatMarkerData marker_trajectories = marker_file.getMarkerTrajectories(marker_label.c_str());
			pos(0,j) = marker_trajectories.x[i] * 1.0e-3;
			pos(1,j) = marker_trajectories.y[i] * 1.0e-3;
			pos(2,j) = marker_trajectories.z[i] * 1.0e-3;
			if (!first_frame_complete) 
				extension->setMarkerLabel(j, marker_label);
			j++;
		}
		first_frame_complete = true;

		extension->addMocapMarkerFrame(current_frame_time, pos);
		current_frame_time += frame_rate;
	}

	return extension;
}

void MotionMarkerPlugin::reload(RBDLModelWrapper* model) {
	addModelMarkersToModel(model);

	for (auto it = model_file_map.begin(); it != model_file_map.end(); it++) {
		if ( it->first == model ) {
			auto ext = loadMotionMarkerFile(it->second);
			model->addExtension(ext);
			parentApp->getToolkitTimeline()->setMaxTime(ext->getMaxTime());
		}
	}
}
