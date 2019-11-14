#include "RenderPlugin.h"

#include <Qt3DExtras>

RenderPlugin::RenderPlugin() {
	parentApp = NULL;
}

RenderPlugin::~RenderPlugin() {

}

void RenderPlugin::init(ToolkitApp* app) {
	parentApp = app;

	render_menu.setTitle("Render");

	parentApp->addMenu(&render_menu);
}
