#include "ForceTorquePlugin.h"
#include <iostream>
#include <clocale>

#include <QFileDialog>
#include <QMessageBox>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QTransform>

#include <parser.hpp>

using namespace RigidBodyDynamics::Math;

ForceTorquePlugin::ForceTorquePlugin() {
	parentApp = NULL;
}

ForceTorquePlugin::~ForceTorquePlugin() {
}

void ForceTorquePlugin::init(ToolkitApp* app) {
	//save reference to parent ToolkitApp 
	parentApp = app;

	load_file_trigger = new QAction("Load Forces/Torques");
	parentApp->addFileAction(load_file_trigger);

	connect(load_file_trigger, SIGNAL(triggered(bool)), this, SLOT(action_load_data()));

	getCSVSettings();
	getArrowSettings();

	QString force_arrow_src = findFile(QString("force_arrow.obj"));
	force_arrow_mesh = new Qt3DRender::QMesh;
	force_arrow_mesh->setSource(QUrl::fromLocalFile(force_arrow_src));

	QString torque_arrow_src = findFile(QString("torque_arrow.obj"));
	torque_arrow_mesh = new Qt3DRender::QMesh;
	torque_arrow_mesh->setSource(QUrl::fromLocalFile(torque_arrow_src));

	QCommandLineOption force_torque_option( QStringList() << "ft" << "forcetorque",
	                                 "Load Force/Torque files <file>", 
	                                 "file"
	                               );
	parentApp->addCmdOption(force_torque_option, [this](QCommandLineParser& parser){
		auto data_list = parser.values("forcetorque");

		for (int i=0; i<data_list.size(); i++) {
			if (i < parentApp->getLoadedModels()->size() ) {
			}
		}
	});
}
void ForceTorquePlugin::getArrowSettings() {
	parentApp->toolkit_settings.beginGroup("RenderOptions");

	//force color setting
	QVariant val = parentApp->toolkit_settings.value("force.color");
	if (val.isNull()) {
		force_color = QColor::fromRgbF(1., 0., 0., 0.5);
		parentApp->toolkit_settings.setValue("force.color", force_color.rgba());
	} else {
		//read as int because it is saved as one, otherwise it would not load correctly
		//watch out for overflow -> will silently fail because char is uint8
		force_color = QColor::fromRgba(val.toUInt());
	}

	//torque color setting
	val = parentApp->toolkit_settings.value("torque.color");
	if (val.isNull()) {
		torque_color = QColor::fromRgbF(0., 1., 0., 0.5);
		parentApp->toolkit_settings.setValue("torque.color", torque_color.rgba());
	} else {
		//read as int because it is saved as one, otherwise it would not load correctly
		//watch out for overflow -> will silently fail because char is uint8
		torque_color = QColor::fromRgba(val.toUInt());
	}

	parentApp->toolkit_settings.endGroup();
}

void ForceTorquePlugin::getCSVSettings() {
	parentApp->toolkit_settings.beginGroup("FileReaderOptions");

	//seperator setting
	QVariant val = parentApp->toolkit_settings.value("csv.seperator");
	if (val.isNull()) {
		csv_seperator = ',';
		parentApp->toolkit_settings.setValue("csv.seperator", csv_seperator);
	} else {
		//read as int because it is saved as one, otherwise it would not load correctly
		//watch out for overflow -> will silently fail because char is uint8
		csv_seperator = val.toInt();
	}

	//trimming setting
	val = parentApp->toolkit_settings.value("csv.trim");
	if (val.isNull()) {
		csv_trim = false;
		parentApp->toolkit_settings.setValue("csv.trim", csv_trim);
	} else {
		csv_trim = val.toBool();
	}

	parentApp->toolkit_settings.endGroup();
}

void ForceTorquePlugin::action_load_data() {
	ArrowFieldModelExtention *field = new ArrowFieldModelExtention(force_arrow_mesh, "Forces", force_color);

	Matrix3fd pos;
	pos.resize(3,1);
	pos(0,0) = 0;
	pos(1,0) = 0;
	pos(2,0) = 0;
	Matrix3fd dir;
	dir.resize(3,1);
	dir(0,0) = 1;
	dir(1,0) = 0;
	dir(2,0) = 1;

	field->addArrowFieldFrame(0, pos, dir);
	parentApp->getLoadedModels()->at(0)->addExtention(field);
}

void ForceTorquePlugin::reload_files() {

}

void ForceTorquePlugin::loadForceTorqueFile(QString path) {

}
