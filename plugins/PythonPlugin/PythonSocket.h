#ifndef PYTHON_SOCKET_INCLUDED
#define PYTHON_SOCKET_INCLUDED

#include <QLocalServer>
#include <QLocalSocket>

#include "EmbeddedPython.h"

class PythonSocketServer : public QLocalServer {
	Q_OBJECT
	public:
		explicit PythonSocketServer(EmbeddedPython* ep, QObject* parent=NULL);
		~PythonSocketServer();

		void startServer(QString name);
	protected:
		void incomingConnection(quintptr socket_descriptor);
	private:
		EmbeddedPython* embedded_python;

		std::vector<PythonThread*> py_threads;
};

class PythonLocalSocket : public QLocalSocket  {
	Q_OBJECT
	public:
		PythonLocalSocket(QObject *parent=NULL);
		~PythonLocalSocket();

		void py_write(std::string str);
		std::string py_read(int len);
		std::string py_readline();
		void py_flush();
		bool is_closed();
};

#endif
