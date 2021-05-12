#ifndef EMBEDDED_PYTHON_H
#define EMBEDDED_PYTHON_H

#include <QObject>
#include <ToolkitApp.h>
#include <QLocalSocket>
#include <QThread>

class PythonLocalSocket;
class EmbeddedPythonImpl;

class EmbeddedPython : public QObject {
	Q_OBJECT

	public:
		EmbeddedPython(ToolkitApp* toolkit);
		~EmbeddedPython();

		void run_code(QString py_code, PythonLocalSocket* callback_socket);
		void run_source(QString python_file);
	private:
		EmbeddedPythonImpl* impl;
};

#endif
