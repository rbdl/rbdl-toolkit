#ifndef CAMERAOPERATORPLUGIN_H_INCLUDED
#define CAMERAOPERATORPLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>
#include <QMenu>

#include <vector>

#include "toolkit_interfaces.h"

class CameraOperatorPlugin : public QObject, public CoreInterface {
	Q_OBJECT
	Q_INTERFACES(CoreInterface)
	Q_PLUGIN_METADATA(IID CoreInterface_iid FILE "metadata.json")

	public:
		CameraOperatorPlugin();
		virtual ~CameraOperatorPlugin();

		void init(ToolkitApp* app);

	private:
		ToolkitApp* parentApp;

		QMenu* camera_menu;

		//default positon settings of camera plugin
		bool camera_reset_enabled;
		float default_distance;
		QVector3D camera_poi;
		QVector3D camera_pos;

		void loadCameraSettings();

	public slots:
		void setOrthographicView(bool state);
		void setTopView();
		void setSideView();
		void setFrontView();

		void reload();

};

#endif 

