#ifndef RENDERPLUGIN_H_INCLUDED
#define RENDERPLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>
#include <QMenu>
#include <QWindow>
#include <QSurface>
#include <QSurfaceFormat>

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

		QWindow* offscreen_render;

	public slots:
		void action_render_image();
};

#endif 

