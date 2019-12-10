#ifndef RENDERPLUGIN_H_INCLUDED
#define RENDERPLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>
#include <QMenu>
#include <QWindow>
#include <QSurface>
#include <QSurfaceFormat>
#include <QPainter>

#include "RenderImageDialog.h"
#include "RenderImageSeriesDialog.h"
#include "RenderVideoDialog.h"

#include "RenderImageDialog.h"
#include "RenderImageSeriesDialog.h"
#include "RenderVideoDialog.h"

#include <QProgressDialog>
#include <Qt3DRender>

#include "QVideoEncoder.h"

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

		QAction* render_image;
		QAction* render_image_series;
		QAction* render_video;

		Qt3DRender::QRenderCaptureReply* capture_reply;
		QColor standard_clear_color;
		QPainter image_composer;
		QImage saved_frame;

		QString file_loc;
		bool last_frame_captured;
		int current_frame;
		unsigned frame_count;
		float current_time;
		float timestep;
		bool do_compositon;
		bool render_transparent;

		QProgressDialog* pbar;
		QVideoEncoder* encoder;

		void init_offscreen_render_surface(int width, int height);

	public slots:
		void action_render_image();
		void action_render_image_series();
		void action_render_video();
		void timelineChange(float max_time);

		void handle_video_frame();
		void handle_image_series_frame();
};

#endif 

