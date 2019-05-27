#include "ToolkitApp.h"
#include "render_util.h"
#include "rbdl_wrapper.h"

ToolkitApp::ToolkitApp(QWidget *parent) {
	main_menu_bar = new QMenuBar(NULL);
	view_menu = main_menu_bar->addMenu("Views");

	this->setMenuBar(main_menu_bar);

	main_display = new SceneWidget(this);
	this->setCentralWidget(main_display);

	main_display->addSceneObject(createGridFloor(-15., 15., 32));

	RBDLModelWrapper model;
	main_display->addSceneObject(model.loadFromFile("/home/judge/Work/Spexsor/data/testModel.lua"));
}

void ToolkitApp::addView(QString name, QWidget *view_widget, Qt::DockWidgetArea area) {
	QDockWidget* dock = new QDockWidget(name, this);
	dock->setWidget(view_widget);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	addDockWidget(area, dock);

	view_widgets.push_back(dock);
	view_menu->addAction(dock->toggleViewAction());
}
