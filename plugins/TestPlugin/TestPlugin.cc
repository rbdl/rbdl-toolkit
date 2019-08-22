#include "TestPlugin.h"

#include <QLabel>
#include <QVBoxLayout>

TestPlugin::TestPlugin() {
	view = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(new QLabel("Test"));
	view->setLayout(layout);
	view_name = "Test";
}

TestPlugin::~TestPlugin() {
}

QString TestPlugin::getViewName() {
	return view_name;
}

QWidget* TestPlugin::getViewWidget() {
	return view;
}
