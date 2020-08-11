#ifndef ANIMATION_PLUGIN_H_INCLUDED
#define ANIMATION_PLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>

#include <vector>

#include "toolkit_interfaces.h"
#include "AnimationModelExtension.h"

class AnimationPlugin : public QObject, public CoreInterface {
	Q_OBJECT
	Q_INTERFACES(CoreInterface)
	Q_PLUGIN_METADATA(IID CoreInterface_iid FILE "metadata.json")

	public:
		AnimationPlugin();
		virtual ~AnimationPlugin();

		void init(ToolkitApp* app);

		AnimationModelExtension* loadAnimationFile(QString path);
	private:
		ToolkitApp* parentApp;
		QAction load_file_trigger;
		QAction export_animation_data;

		char csv_seperator;
		bool csv_trim;

		void getCSVSettings();
		std::map<RBDLModelWrapper*, QString> model_file_map;
	public slots:
		void action_load_animation();
		void action_export_animation();
		void reload(RBDLModelWrapper* model);
};

#endif 

