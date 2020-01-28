#ifndef FORCETORQUE_PLUGIN_H_INCLUDED
#define FORCETORQUE_PLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>

#include <vector>

#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender/QMesh>

#include "toolkit_interfaces.h"
#include "util.h"

#include "ArrowFieldModelExtention.h"

class ForceTorquePlugin : public QObject, public OptionalInterface{
	Q_OBJECT
	Q_INTERFACES(OptionalInterface)
	Q_PLUGIN_METADATA(IID OptionalInterface_iid FILE "metadata.json")

	public:
		ForceTorquePlugin();
		virtual ~ForceTorquePlugin();

		void init(ToolkitApp* app);

		void loadForceTorqueFile(QString path, ArrowFieldModelExtention* force_field, ArrowFieldModelExtention* torque_field);

	private:
		ToolkitApp* parentApp;
		QAction* load_file_trigger;

		Qt3DRender::QMesh* force_arrow_mesh;
		Qt3DRender::QMesh* torque_arrow_mesh;

		float draw_threshold;
		float arrow_scale_factor;

		QColor force_color;
		QColor torque_color;

		char csv_seperator;
		bool csv_trim;

		void getCSVSettings();
		void getArrowSettings();

	public slots:
		void action_load_data();
		void reload_files();

};

#endif 

