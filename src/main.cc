#include <QApplication>

#include "ToolkitApp.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[])
{
	putenv("QT_LOGGING_RULES=qt5ct.debug=false");
	QApplication app(argc, argv);

	QCoreApplication::setOrganizationName("ORB");
	QCoreApplication::setApplicationName("rbdl-toolkit");
	QCoreApplication::setApplicationVersion(TOOKIT_VERSION);

	ToolkitApp *rbdl_toolkit = new ToolkitApp;
	rbdl_toolkit->parseCmd(app);
	rbdl_toolkit->show();

	return app.exec();
}
