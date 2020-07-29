#include "PythonPlugin.h"
#include <iostream>


#include <QCommandLineParser>
#include <QCommandLineOption>


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

	

	//std::cout << "PythonPlugin loaded" << std::endl;
}







