#include "VisualExtrasPlugin.h"
#include <iostream>





VisualExtrasPlugin::VisualExtrasPlugin() {
	parentApp = NULL;
}

VisualExtrasPlugin::~VisualExtrasPlugin() {
}

void VisualExtrasPlugin::init(ToolkitApp* app) {
	//save reference to parent ToolkitApp 
	parentApp = app;

	

	

	loadVisualExtrasSettings();

	std::cout << "VisualExtrasPlugin loaded" << std::endl;

	connect(parentApp, &ToolkitApp::reloaded_model, this, &VisualExtrasPlugin::reload);
}

void VisualExtrasPlugin::loadVisualExtrasSettings() {
	parentApp->toolkit_settings.beginGroup("VisualExtrasOptions");

	// example read/write of setting

	//QVariant val = parentApp->toolkit_settings.value("marker.color");
	//if (val.isNull()) {
	//	marker_color = QColor::fromRgbF(0., 0., 1., 1.);
	//	parentApp->toolkit_settings.setValue("marker.color", marker_color.rgba());
	//} else {
	//	marker_color = QColor::fromRgba(val.toUInt());
	//}
	//parentApp->toolkit_settings.setType("marker.color", marker_color);

	parentApp->toolkit_settings.endGroup();
}





void VisualExtrasPlugin::reload(RBDLModelWrapper *model) {
	
}