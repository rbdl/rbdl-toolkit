#ifndef PYTHONPLUGIN_H_INCLUDED
#define PYTHONPLUGIN_H_INCLUDED
#include <QWidget>
#include <QString>
#include <QApplication>

#include "toolkit_interfaces.h"
#include "toolkit_errors.h"
#include "util.h"

#include <qtermwidget5/qtermwidget.h>

#include <EmbeddedPython.h>
#include <PythonSocket.h>


class ToolkitSocketTerminal : public QTermWidget {
	Q_OBJECT
	public:
		ToolkitSocketTerminal(const QString socket_addr, QWidget *parent=NULL);
		~ToolkitSocketTerminal();
	public Q_SLOTS:
		void atError(QLocalSocket::LocalSocketError err);
	private:
		QLocalSocket *socket;
};

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

		EmbeddedPython* embedded_python;
		PythonSocketServer* python_server;
		ToolkitSocketTerminal* console;

};

#endif 
