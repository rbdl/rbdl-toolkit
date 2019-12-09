#include "RenderPlugin.h"

#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <QProgressDialog>

#include <iostream>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

RenderPlugin::RenderPlugin() {
	parentApp = NULL;
	offscreen_render = nullptr;

	render_image_dialog = nullptr;
	render_imageseries_dialog = nullptr;
	render_video_dialog = nullptr;
}

RenderPlugin::~RenderPlugin() {
	if (render_image_dialog != nullptr) 
		delete render_image_dialog;

	if (render_imageseries_dialog != nullptr) 
		delete render_imageseries_dialog;

	if (render_video_dialog != nullptr) 
		delete render_video_dialog;
}

void RenderPlugin::init(ToolkitApp* app) {
	parentApp = app;

	render_image_dialog = new RenderImageDialog(parentApp);
	render_imageseries_dialog= new RenderImageSeriesDialog(parentApp);
	render_video_dialog = new RenderVideoDialog(parentApp);

	render_menu.setTitle("Render");

	render_image = render_menu.addAction("Render Image");
	connect(render_image, &QAction::triggered, this, &RenderPlugin::action_render_image);

	render_image_series = render_menu.addAction("Render Image Series");
	connect(render_image_series, &QAction::triggered, this, &RenderPlugin::action_render_image_series);

	render_video = render_menu.addAction("Render Video");
	connect(render_video, &QAction::triggered, this, &RenderPlugin::action_render_video);

	float duration = parentApp->getToolkitTimeline()->getMaxTime();
	timelineChange(duration);

	connect(parentApp->getToolkitTimeline(), &ToolkitTimeline::maxTimeChanged, this, &RenderPlugin::timelineChange);

	parentApp->addMenu(&render_menu);
}

void RenderPlugin::init_offscreen_render_surface(int width, int height) {
	offscreen_render = new QWindow();
	offscreen_render->setObjectName(QLatin1String("QOffscreenSurface"));

	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
	format.setSwapBehavior(QSurfaceFormat::SingleBuffer);

	offscreen_render->setFormat(format);
	offscreen_render->setSurfaceType(QSurface::OpenGLSurface);

	offscreen_render->setVisibility(QWindow::Hidden);
	offscreen_render->setGeometry(0, 0, width, height);
	offscreen_render->create();
}

void RenderPlugin::timelineChange(float max_time) {
	if (max_time == 0.) {
		render_image_series->setDisabled(true);
		render_video->setDisabled(true);
	} else {
		render_image_series->setDisabled(false);
		render_video->setDisabled(false);
	}
}

void RenderPlugin::action_render_image() {
	render_image_dialog->WidthSpinBox->setValue(parentApp->getSceneObj()->getWidth());
	render_image_dialog->HeightSpinBox->setValue(parentApp->getSceneObj()->getHeight());
	render_image_dialog->setFilename(QString("image.png"));

	int result = render_image_dialog->exec();

	if (result == QDialog::Rejected)
		return;

	int w = render_image_dialog->WidthSpinBox->value();
	int h = render_image_dialog->HeightSpinBox->value();
	QString filename = render_image_dialog->filenameEdit->text();

	init_offscreen_render_surface(w, h);

    QColor standard_clear_color = parentApp->getSceneObj()->getDefaultClearColor();
	if (render_image_dialog->TransparentBackgroundCheckBox->isChecked()) {
		parentApp->getSceneObj()->setClearColor(QColor("transparent"));
	} else {
		parentApp->getSceneObj()->setClearColor(standard_clear_color);
	}
	parentApp->getSceneObj()->setOffscreenRender(offscreen_render);


	capture_reply = parentApp->getSceneObj()->requestFrameCapture();
	connect(capture_reply, &QRenderCaptureReply::completed, [=]
	        {
		         QImage rendered_image = capture_reply->image();
	             rendered_image.save(filename);
	             delete capture_reply;
	             parentApp->getSceneObj()->setClearColor(standard_clear_color);
	             parentApp->getSceneObj()->setOffscreenRender(nullptr);
	             delete offscreen_render;
		    });

}

void RenderPlugin::action_render_image_series() {
	int fps;
	bool fps_mode;
	bool doComposite;
	bool render_transparent;
	int width;
	int height;

	int result = render_imageseries_dialog->exec();

	if (result == QDialog::Rejected)
		return;

	width = render_imageseries_dialog->WidthSpinBox->value();
	height = render_imageseries_dialog->HeightSpinBox->value();
	fps = render_imageseries_dialog->FpsSpinBox->value();

	if (render_imageseries_dialog->fpsModeRadioButton->isChecked())
		fps_mode = true;
	else
		fps_mode = false;

	doComposite = render_imageseries_dialog->compositeBox->isChecked();
	render_transparent = render_imageseries_dialog->transparentBackgroundCheckBox->isChecked();

	file_loc = render_imageseries_dialog->filenameEdit->text();

	init_offscreen_render_surface(width, height);
	parentApp->getSceneObj()->setOffscreenRender(offscreen_render);

	pbar = new QProgressDialog("Rendering offscreen", "Abort Render", 0, frame_count, parentApp);
	pbar->setWindowModality(Qt::WindowModal);
	pbar->setMinimumDuration(0);

	last_frame_captured = false;
	current_frame = 0;
	current_time = (float) current_frame * timestep;

	parentApp->getToolkitTimeline()->setCurrentTime(current_time, true);

	capture_reply = parentApp->getSceneObj()->requestFrameCapture();
	connect(capture_reply, &QRenderCaptureReply::completed, this, &RenderPlugin::handle_image_series_frame);
}

void RenderPlugin::handle_image_series_frame() {
		pbar->setValue(current_frame);
		QImage rendered_image = capture_reply->image().convertToFormat(QImage::Format_ARGB32);

		if (last_frame_captured) {
			delete offscreen_render;
			return;
		}

		//update scene for next frame
		current_frame++;
		current_time = (float)current_frame * timestep;
		for (auto model : *(parentApp->getLoadedModels()) ) {
			model->model_update(current_time);
		}

		if (pbar->wasCanceled()) {
			parentApp->getSceneObj()->setOffscreenRender(nullptr);
			delete offscreen_render;
			delete capture_reply;
			return;
		}

		delete capture_reply;
		if (!last_frame_captured) {
			capture_reply = parentApp->getSceneObj()->requestFrameCapture();
			connect(capture_reply, &QRenderCaptureReply::completed, this, &RenderPlugin::handle_image_series_frame);
		}

		last_frame_captured = (current_frame == frame_count);
}

void RenderPlugin::action_render_video() {
	unsigned width;
	unsigned height;
	double length;
	unsigned fps = 25;
	QString filename;
	float duration = parentApp->getToolkitTimeline()->getMaxTime();

	render_video_dialog->set_video_lenght(duration);
	int result = render_video_dialog->exec();

	if (result == QDialog::Rejected)
		return;

	width = render_video_dialog->WidthSpinBox->value();
	height = render_video_dialog->HeightSpinBox->value();
	length = render_video_dialog->TimeSpinBox->value(); 
	filename = render_video_dialog->filenameEdit->text();

	init_offscreen_render_surface(width, height);
	parentApp->getSceneObj()->setOffscreenRender(offscreen_render);

	frame_count = fps * (length);
	timestep = duration / frame_count;

	pbar = new QProgressDialog("Rendering offscreen", "Abort Render", 0, frame_count, parentApp);
	pbar->setWindowModality(Qt::WindowModal);
	pbar->setMinimumDuration(0);

	encoder = new QVideoEncoder;
	encoder->createFile(filename, width, height, fps);
	last_frame_captured = false;
	current_frame = 0;
	current_time = (float) current_frame * timestep;

	parentApp->getToolkitTimeline()->setCurrentTime(current_time, true);

	capture_reply = parentApp->getSceneObj()->requestFrameCapture();
	connect(capture_reply, &QRenderCaptureReply::completed, this, &RenderPlugin::handle_video_frame);
}

void RenderPlugin::handle_video_frame() {
		pbar->setValue(current_frame);
		QImage rendered_image = capture_reply->image().convertToFormat(QImage::Format_ARGB32);
		encoder->encodeImage(rendered_image);

		if (last_frame_captured) {
			rendered_image.save("debug.png");
			for (int i=0; i<24; i++) {
				encoder->encodeImage(rendered_image);
			}
			encoder->close();
			parentApp->getSceneObj()->setOffscreenRender(nullptr);
			delete offscreen_render;
			delete encoder;
			return;
		}

		//update scene for next frame
		current_frame++;
		current_time = (float)current_frame * timestep;
		for (auto model : *(parentApp->getLoadedModels()) ) {
			model->model_update(current_time);
		}

		if (pbar->wasCanceled()) {
			parentApp->getSceneObj()->setOffscreenRender(nullptr);
			delete offscreen_render;
			delete encoder;
			delete capture_reply;
			return;
		}

		delete capture_reply;
		if (!last_frame_captured) {
			capture_reply = parentApp->getSceneObj()->requestFrameCapture();
			connect(capture_reply, &QRenderCaptureReply::completed, this, &RenderPlugin::handle_video_frame);
		}

		last_frame_captured = (current_frame == frame_count);
}
