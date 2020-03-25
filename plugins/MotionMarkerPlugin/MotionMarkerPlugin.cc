#include "MotionMarkerPlugin.h"
#include <iostream>
#include <clocale>

#include <QFileDialog>
#include <QMessageBox>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QEntity>

#include <parser.hpp>
#include <toolkit_errors.h>
#include <rbdl_wrapper.h>

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
	});

	marker_mesh.setSource(QUrl::fromLocalFile(findFile(QString("unit_sphere_medres.obj"))));
	loadMarkerSettings();

	//display model markers of all loaded models
	auto all_models = parentApp->getLoadedModels();
	for (auto model : *all_models) {
		addModelMarkersToModel(model);
	}

	connect(parentApp, SIGNAL(model_loaded(RBDLModelWrapper*)), this, SLOT(addModelMarkersToModel(RBDLModelWrapper*)));
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

void MotionMarkerPlugin::addModelMarkersToModel(RBDLModelWrapper *model) {
	unsigned int segment_cnt = model->model_luatable["frames"].length();
	for(int i=1; i<=segment_cnt; i++) {
		std::string segment_name = model->model_luatable["frames"][i]["name"].get<std::string>();
		std::vector<LuaKey> keys= model->model_luatable["frames"][i]["markers"].keys();

		for (auto marker_name : keys) {
			Qt3DCore::QEntity* marker_visual = new Qt3DCore::QEntity;
			marker_visual->setProperty("Scene.ObjGroup", QVariant("Model Marker"));

			Vector3d marker_position = model->model_luatable["frames"][i]["markers"][marker_name.string_value.c_str()].getDefault(Vector3d(0.,0.,0.));
			marker_position = model->axis_transform * marker_position;

			Qt3DExtras::QPhongAlphaMaterial* material = new Qt3DExtras::QPhongAlphaMaterial;
			material->setAmbient(marker_color_model);
			material->setAlpha((float)marker_color_model.alpha() / 255.);

			Qt3DCore::QTransform* position = new Qt3DCore::QTransform;
			position->setTranslation(QVector3D(marker_position[0], marker_position[1], marker_position[2]));
			position->setScale(marker_size);

			marker_visual->addComponent(&marker_mesh);
			marker_visual->addComponent(position);
			marker_visual->addComponent(material);

			model->addVisual(segment_name, marker_visual);
		}
	}
}

void MotionMarkerPlugin::action_load_data() {
}

void MotionMarkerPlugin::loadMotionMarkerFile(QString path) {
}
