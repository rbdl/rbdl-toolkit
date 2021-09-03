#ifndef FORCETORQUE_PLUGIN_H_INCLUDED
#define FORCETORQUE_PLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>

#include <vector>

#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender/QMesh>

#include "toolkit_interfaces.h"
#include "toolkit_config.h"
#include "util.h"

#include "ArrowFieldModelExtension.h"

class TOOLKIT_PLUGIN_DLLAPI ForceTorquePlugin : public QObject, public CoreInterface{
	Q_OBJECT
	Q_INTERFACES(CoreInterface)
	Q_PLUGIN_METADATA(IID CoreInterface_iid FILE "metadata.json")

	public:
		ForceTorquePlugin();
		virtual ~ForceTorquePlugin();

		void init(ToolkitApp* app);

		void loadForceTorqueFile(QString path, ArrowFieldModelExtension* force_field, ArrowFieldModelExtension* torque_field);

	private:
		ToolkitApp* parentApp;
		QAction* load_file_trigger;

		std::map<RBDLModelWrapper*, QString> model_file_map;

		QString force_arrow_src;
		QString torque_arrow_src;

		float draw_threshold;
		float arrow_scale_factor;
		bool  pos_at_tip;

		QColor force_color;
		QColor torque_color;

		char csv_seperator;
		bool csv_trim;

		void getCSVSettings();
		void getArrowSettings();

	public slots:
		void action_load_data();
		void reload(RBDLModelWrapper* model);

};

#endif 

