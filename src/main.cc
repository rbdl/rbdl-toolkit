#include <QApplication>

#include "ToolkitApp.h"

#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	ToolkitApp *rbdl_toolkit = new ToolkitApp;
	rbdl_toolkit->show();

	return app.exec();
}
