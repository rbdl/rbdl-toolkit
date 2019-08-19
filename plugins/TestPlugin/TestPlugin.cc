#include "TestPlugin.h"

TestPlugin::TestPlugin() {
	view = NULL;
	view_name = "Test";
}

QString TestPlugin::getViewName() {
	return view_name;
}

QWidget* TestPlugin::getViewWidget() {
	return view;
}
