#include "ToolkitApp.h"

ToolkitApp::ToolkitApp(QWidget *parent) {
	main_display = new SceneWidget(this);
	this->setCentralWidget(main_display);

	main_display->addSceneObject(createGridFloor(-15., 15., 32));
}
