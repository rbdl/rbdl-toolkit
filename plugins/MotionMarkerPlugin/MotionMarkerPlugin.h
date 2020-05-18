#ifndef MOTIONMARKER_PLUGIN_H_INCLUDED
#define MOTIONMARKER_PLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>

#include <vector>

#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender/QMesh>

#include "toolkit_interfaces.h"
#include "util.h"

#include "MotionMarkerExtention.h"
#include "ModelMarkerExtention.h"

class MotionMarkerPlugin : public QObject, public OptionalInterface{
	Q_OBJECT
	Q_INTERFACES(OptionalInterface)
	Q_PLUGIN_METADATA(IID OptionalInterface_iid FILE "metadata.json")

	public:
		MotionMarkerPlugin();
		virtual ~MotionMarkerPlugin();

		void init(ToolkitApp* app);

		MotionMarkerExtention* loadMotionMarkerFile(QString path);

	private:
		ToolkitApp* parentApp;
		QAction* load_file_trigger;

		Qt3DRender::QMesh marker_mesh;
		QColor marker_color_model;
		QColor marker_color;
		float marker_size;

		void loadMarkerSettings();

	public slots:
		void action_load_data();
		void addModelMarkersToModel(RBDLModelWrapper* model);

};

#endif 

