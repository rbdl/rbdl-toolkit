#ifndef PYTHONPLUGIN_H_INCLUDED
#define PYTHONPLUGIN_H_INCLUDED
#include <QWidget>
#include <QString>
#include <QApplication>

#include "toolkit_interfaces.h"
#include "toolkit_errors.h"
#include "util.h"

#include <qtermwidget5/qtermwidget.h>



class PythonPlugin : public QObject, public OptionalInterface {
	Q_OBJECT
	Q_INTERFACES(OptionalInterface)
	Q_PLUGIN_METADATA(IID OptionalInterface_iid FILE "metadata.json")
	public:
		PythonPlugin();
		virtual ~PythonPlugin();

		void init(ToolkitApp* app);
		
	private:
		ToolkitApp* parentApp;

};

#endif 
