#ifndef MOTIONMARKER_PLUGIN_H_INCLUDED
#define MOTIONMARKER_PLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>

#include <vector>

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

		QColor marker_color_model;
		QColor marker_color;
		float marker_size;

		void loadMarkerSettings();

		std::map<RBDLModelWrapper*, QString> model_file_map;
	public slots:
		void action_load_data();
		void addModelMarkersToModel(RBDLModelWrapper* model);
		void reload(RBDLModelWrapper* model);

};

#endif 

