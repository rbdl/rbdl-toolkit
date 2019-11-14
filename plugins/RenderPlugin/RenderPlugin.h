#ifndef RENDERPLUGIN_H_INCLUDED
#define RENDERPLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>
#include <QMenu>

#include "toolkit_interfaces.h"

class RenderPlugin : public QObject, public OptionalInterface {
	Q_OBJECT
	Q_INTERFACES(OptionalInterface)
	Q_PLUGIN_METADATA(IID OptionalInterface_iid FILE "metadata.json")

	public:
		RenderPlugin();
		~RenderPlugin();
		void init(ToolkitApp* app);

		QMenu render_menu;

	private:
		ToolkitApp* parentApp;
};

#endif 

