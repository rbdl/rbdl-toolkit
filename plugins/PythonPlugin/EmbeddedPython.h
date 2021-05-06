#ifndef EMBEDDED_PYTHON_H
#define EMBEDDED_PYTHON_H

#include <QObject>
#include <ToolkitApp.h>
#include <QLocalSocket>
#include <QThread>

class PythonLocalSocket;

class EmbeddedPython : public QObject {
	Q_OBJECT

	public:
		EmbeddedPython(ToolkitApp* toolkit);
		~EmbeddedPython();

		void startPythonShell(PythonLocalSocket* socket);
	private:
		ToolkitApp *toolkit;
};

class PythonThread : public QThread {
	Q_OBJECT
	public:
		explicit PythonThread(EmbeddedPython* ep, quintptr ID, QObject* parent = NULL);

		void run();

	public Q_SLOTS:
		void disconnected();

	private:
		PythonLocalSocket* socket;
		quintptr socket_descriptor;
		EmbeddedPython* embedded_python;
};

#endif
