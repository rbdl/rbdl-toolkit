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

	parentApp->deleteMenu(render_menu);
}

void RenderPlugin::init(ToolkitApp* app) {
	parentApp = app;

	render_image_dialog = new RenderImageDialog(parentApp);
	render_imageseries_dialog= new RenderImageSeriesDialog(parentApp);
	render_video_dialog = new RenderVideoDialog(parentApp);

	render_menu = app->getMenu("Render");

	render_image = render_menu->addAction("Render Image");
	connect(render_image, &QAction::triggered, this, &RenderPlugin::action_render_image);

	render_image_series = render_menu->addAction("Render Image Series");
	connect(render_image_series, &QAction::triggered, this, &RenderPlugin::action_render_image_series);

	render_video = render_menu->addAction("Render Video");
	connect(render_video, &QAction::triggered, this, &RenderPlugin::action_render_video);

	float duration = parentApp->getToolkitTimeline()->getMaxTime();
	timelineChange(duration);

	connect(parentApp->getToolkitTimeline(), &ToolkitTimeline::maxTimeChanged, this, &RenderPlugin::timelineChange);
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

	standard_clear_color = parentApp->getSceneObj()->getDefaultClearColor();
	if (render_image_dialog->TransparentBackgroundCheckBox->isChecked()) {
		parentApp->getSceneObj()->setClearColor(QColor("transparent"));
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
	int width;
	int height;
	float duration = parentApp->getToolkitTimeline()->getMaxTime();

	int result = render_imageseries_dialog->exec();

	if (result == QDialog::Rejected)
		return;

	width = render_imageseries_dialog->WidthSpinBox->value();
	height = render_imageseries_dialog->HeightSpinBox->value();
	fps = render_imageseries_dialog->FpsSpinBox->value();
	do_compositon = render_imageseries_dialog->compositeBox->isChecked();
	render_transparent = render_imageseries_dialog->transparentBackgroundCheckBox->isChecked();
	file_loc = render_imageseries_dialog->filenameEdit->text();

	if (render_imageseries_dialog->fpsModeRadioButton->isChecked())
		fps_mode = true;
	else
		fps_mode = false;

	if (fps_mode) {
		frame_count = duration / (float)fps;
	} else {
		frame_count = fps;
	}

	timestep = duration / (float)frame_count;

	init_offscreen_render_surface(width, height);
	if (do_compositon) {
		saved_frame = QImage(width, height, QImage::Format_ARGB32);
		image_composer.begin(&saved_frame);
		image_composer.setCompositionMode(QPainter::CompositionMode_Source);
		image_composer.fillRect(saved_frame.rect(), Qt::transparent);
		image_composer.setCompositionMode(QPainter::CompositionMode_Plus);
	}

	if (render_transparent) {
		parentApp->getSceneObj()->setClearColor(QColor("transparent"));
	}
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
		QString save_path = QDir(file_loc).filePath(QString("image-series-%1.png").arg((int)current_frame, 6, 10, QLatin1Char('0')));
		rendered_image.save(save_path);

		if (do_compositon) {
			image_composer.drawImage(0, 0, rendered_image);
		}

		if (last_frame_captured) {
			if (do_compositon) {
				image_composer.end();
				QString composed_path = QDir(file_loc).filePath(QString("image-series-%1.png").arg("composed"));
				saved_frame.save(composed_path);
			}
	        parentApp->getSceneObj()->setClearColor(standard_clear_color);
			parentApp->getSceneObj()->setOffscreenRender(nullptr);
			delete offscreen_render;
			delete capture_reply;
			return;
		}

		//update scene for next frame
		current_frame++;
		current_time = (float)current_frame * timestep;
		for (auto model : *(parentApp->getLoadedModels()) ) {
			model->model_update(current_time);
		}

		if (pbar->wasCanceled()) {
	        parentApp->getSceneObj()->setClearColor(standard_clear_color);
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

	encoder = new QVideoWriter;
	encoder->createVideo(filename, width, height, fps);
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
		encoder->addFrame(rendered_image);

		if (last_frame_captured) {
			for (int j=0;j<7;j++)
				encoder->addFrame(rendered_image);

			encoder->finish();
			parentApp->getSceneObj()->setOffscreenRender(nullptr);
			delete capture_reply;
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
