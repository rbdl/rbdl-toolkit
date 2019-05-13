#include "ToolkitApp.h"
#include "render_util.h"

ToolkitApp::ToolkitApp(QWidget *parent) {
	main_display = new SceneWidget(this);
	this->setCentralWidget(main_display);

	main_menu_bar = new QMenuBar(NULL);
	view_menu = main_menu_bar->addMenu("Views");

	this->setMenuBar(main_menu_bar);

	main_display->addSceneObject(createGridFloor(-15., 15., 32));
}

void ToolkitApp::addView(QString name, QWidget *view_widget) {
	QDockWidget* dock = new QDockWidget(name, this);
	dock->setWidget(view_widget);
	addDockWidget(Qt::RightDockWidgetArea, dock);

	view_menu->addAction(dock->toggleViewAction());
	view_widgets.push_back(dock);
}
