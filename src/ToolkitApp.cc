#include "ToolkitApp.h"

ToolkitApp::ToolkitApp(QWidget *parent) {
	main_display = new SceneWidget(this);
	this->setCentralWidget(main_display);
}
