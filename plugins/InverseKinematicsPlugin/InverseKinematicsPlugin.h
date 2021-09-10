#ifndef IK_PLUGIN_H_INCLUDED
#define IK_PLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>

#include <vector>

#include "toolkit_interfaces.h"
#include "toolkit_config.h"
#include "util.h"

#include "MotionMarkerExtension.h"
#include "ModelMarkerExtension.h"
#include "AnimationModelExtension.h"

#ifdef  TOOLKIT_COMPILE_IKPLUGIN
#define TOOLKIT_IKPLUGIN_DLLAPI TOOLKIT_DLLEXPORT
#else
#define TOOLKIT_IKPLUGIN_DLLAPI TOOLKIT_PLUGIN_DLLAPI
#endif

class TOOLKIT_IKPLUGIN_DLLAPI InverseKinematicsPlugin : public QObject, public OptionalInterface{
	Q_OBJECT
	Q_INTERFACES(OptionalInterface)
	Q_PLUGIN_METADATA(IID OptionalInterface_iid FILE "metadata.json")

	public:
		InverseKinematicsPlugin();
		virtual ~InverseKinematicsPlugin();

		void init(ToolkitApp* app);

	private:
		ToolkitApp* parentApp;

		double tolerance;
		double lambda;
		unsigned max_steps;

		QMenu* compute_menu;
		QAction calculate_animation;

		void loadIKSettings();

	public slots:
		void run_ik();

};

#endif 

