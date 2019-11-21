#include "RenderPlugin.h"

#include "RenderImageDialog.h"

#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>

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

	int result = renderImageDialog->exec();

	if (result == QDialog::Rejected)
		return;

	int w = renderImageDialog->WidthSpinBox->value();
	int h = renderImageDialog->HeightSpinBox->value();

	offscreen_render = new QWindow();
	offscreen_render->setSurfaceType(QSurface::OpenGLSurface);
	offscreen_render->setVisibility(QWindow::Hidden);
	offscreen_render->setGeometry(0,0,w,h);
	offscreen_render->create();

	parentApp->getSceneObj()->setOffscreenRender(offscreen_render);
	QRenderCaptureReply* capture_reply = parentApp->getSceneObj()->requestFrameCapture();

	connect(capture_reply, &QRenderCaptureReply::completed, [=]
	        {
	             capture_reply->saveImage("image00.png");
	             delete capture_reply;
	             delete offscreen_render;
	             parentApp->getSceneObj()->setOffscreenRender(nullptr);
		    });

}
