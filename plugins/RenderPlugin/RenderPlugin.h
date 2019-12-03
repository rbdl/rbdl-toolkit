#ifndef RENDERPLUGIN_H_INCLUDED
#define RENDERPLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>
#include <QMenu>
#include <QWindow>
#include <QSurface>
#include <QSurfaceFormat>

#include "RenderImageDialog.h"
#include "RenderImageSeriesDialog.h"
#include "RenderVideoDialog.h"

#include "RenderImageDialog.h"
#include "RenderImageSeriesDialog.h"
#include "RenderVideoDialog.h"


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

		RenderImageDialog* render_image_dialog;
		RenderImageSeriesDialog* render_imageseries_dialog;
		RenderVideoDialog* render_video_dialog;

		void init_offscreen_render_surface(int width, int height);

	public slots:
		void action_render_image();
		void action_render_image_series();
		void action_render_video();
};

#endif 

