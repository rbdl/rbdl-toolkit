#ifndef TESTPLUGIN_H_INCLUDED
#define TESTPLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>

#include "toolkit_interfaces.h"

class AnimationPlugin : public QObject, public CoreInterface {
	Q_OBJECT
	Q_INTERFACES(CoreInterface)
	Q_PLUGIN_METADATA(IID CoreInterface_iid FILE "metadata.json")

	public:
		AnimationPlugin();
		virtual ~AnimationPlugin();

		void init(ToolkitApp* app);

	private:
		ToolkitApp* parentApp;

		void loadAnimationFile(QString path);

	public slots:
		void action_load_animation();
		void reload_files();

};

#endif 

