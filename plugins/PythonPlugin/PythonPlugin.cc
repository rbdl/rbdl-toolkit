#include "PythonPlugin.h"
#include <iostream>

#include <QCommandLineParser>
#include <QCommandLineOption>

#define PY_SSIZE_T_CLEAN
#include <Python.h>


PythonPlugin::PythonPlugin() {
	parentApp = NULL;
}

PythonPlugin::~PythonPlugin() {
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

	Py_Initialize();
	PyRun_SimpleString("print('Hello World from Python')");
	Py_Finalize();
}







