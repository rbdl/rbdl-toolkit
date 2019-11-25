#include "RenderPlugin.h"

#include "RenderImageDialog.h"

#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>

#include <iostream>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

RenderPlugin::RenderPlugin() {
	parentApp = NULL;
}

RenderPlugin::~RenderPlugin() {

}

void RenderPlugin::init(ToolkitApp* app) {
	parentApp = app;

	render_menu.setTitle("Render");

	QAction* render_image = render_menu.addAction("Render Image");
	connect(render_image, &QAction::triggered, this, &RenderPlugin::action_render_image);

	parentApp->addMenu(&render_menu);
}

void RenderPlugin::action_render_image() {
	RenderImageDialog* renderImageDialog = new RenderImageDialog(parentApp);
	renderImageDialog->WidthSpinBox->setValue(parentApp->getSceneObj()->getWidth());
	renderImageDialog->HeightSpinBox->setValue(parentApp->getSceneObj()->getHeight());
	renderImageDialog->setFilename(QString("image.png"));

	int result = renderImageDialog->exec();

	if (result == QDialog::Rejected)
		return;

	int w = renderImageDialog->WidthSpinBox->value();
	int h = renderImageDialog->HeightSpinBox->value();
	QString filename = renderImageDialog->filenameEdit->text();

	offscreen_render = new QWindow();
	offscreen_render->setSurfaceType(QSurface::OpenGLSurface);

	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
	offscreen_render->setFormat(format);

	offscreen_render->setVisibility(QWindow::Hidden);
	offscreen_render->setGeometry(0,0,w,h);
	offscreen_render->create();
    QColor standard_clear_color = parentApp->getSceneObj()->getDefaultClearColor();

	if (renderImageDialog->TransparentBackgroundCheckBox->isChecked()) {
		parentApp->getSceneObj()->setClearColor(QColor("transparent"));
		parentApp->getSceneObj()->setOffscreenRender(offscreen_render);
	} else {
		parentApp->getSceneObj()->setClearColor(standard_clear_color);
		parentApp->getSceneObj()->setOffscreenRender(offscreen_render);
	}

	QRenderCaptureReply* capture_reply = parentApp->getSceneObj()->requestFrameCapture();

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
