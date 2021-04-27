#include "PythonPlugin.h"
#include <iostream>

#include <QCommandLineParser>
#include <QCommandLineOption>

//#define PY_SSIZE_T_CLEAN
//#include <Python.h>


PythonPlugin::PythonPlugin() {
	parentApp = NULL;

	//Py_Initialize();
}

PythonPlugin::~PythonPlugin() {
	//Py_Finalize();
}

void PythonPlugin::init(ToolkitApp* app) {
	//save reference to parent ToolkitApp 
	parentApp = app;

	QCommandLineOption python_option( QStringList() << "s" <<"python",
	                                 "Load Python skript <file>", 
	                                 "file"
	                               );
	parentApp->addCmdOption(python_option, [this](QCommandLineParser& parser){
		auto data_list = parser.values("python");
		// implement cmd function here
	});

	QTermWidget* console = new QTermWidget(false);
	QFont font = QApplication::font();
#ifdef Q_OS_MACOS
    font.setFamily(QStringLiteral("Monaco"));
#elif defined(Q_WS_QWS)
    font.setFamily(QStringLiteral("fixed"));
#else
    font.setFamily(QStringLiteral("Monospace"));
#endif
	font.setPointSize(12);

    console->setTerminalFont(font);
	console->setShellProgram("/usr/bin/python");
	console->startShellProgram();


	parentApp->addView("Terminal", qobject_cast<QWidget*>(console), Qt::TopDockWidgetArea);


	//Py_Initialize();
	//PyRun_SimpleString("print('Hello World from Python')");
	//Py_Finalize();
}
