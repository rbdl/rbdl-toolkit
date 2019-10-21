#include <QApplication>

#include "ToolkitApp.h"

#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QCoreApplication::setOrganizationName("ORB");
	QCoreApplication::setApplicationName("rbdl-toolkit");

	ToolkitApp *rbdl_toolkit = new ToolkitApp;
	rbdl_toolkit->show();

	return app.exec();
}
