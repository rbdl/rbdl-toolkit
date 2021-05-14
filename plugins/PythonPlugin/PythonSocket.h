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

		std::vector<PythonLocalSocket*> py_sockets;

	public Q_SLOTS:
		void run_code(QString line, PythonLocalSocket* callback_socket);
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
		int py_id();

	Q_SIGNALS:
		void received_line(QString, PythonLocalSocket*);

	public Q_SLOTS:
		void recv_data();
};

#endif
