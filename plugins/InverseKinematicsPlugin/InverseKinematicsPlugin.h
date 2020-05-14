#ifndef IK_PLUGIN_H_INCLUDED
#define IK_PLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>

#include <vector>

#include "toolkit_interfaces.h"
#include "util.h"

#include "MotionMarkerExtention.h"
#include "ModelMarkerExtention.h"
#include "AnimationModelExtention.h"

class InverseKinematicsPlugin : public QObject, public OptionalInterface{
	Q_OBJECT
	Q_INTERFACES(OptionalInterface)
	Q_PLUGIN_METADATA(IID OptionalInterface_iid FILE "metadata.json")

	public:
		InverseKinematicsPlugin();
		virtual ~InverseKinematicsPlugin();

		void init(ToolkitApp* app);

	private:
		ToolkitApp* parentApp;

		QMenu* compute_menu;
		QAction calculate_animation;

	public slots:
		void run_ik();

};

#endif 

