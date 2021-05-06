#include "PythonSocket.h"

/*==============================================================================
 * PythonSocketServer
 *=============================================================================*/

PythonSocketServer::PythonSocketServer(EmbeddedPython* ep, QObject* parent)
	: QLocalServer(parent), embedded_python(ep) {}

PythonSocketServer::~PythonSocketServer() {
	for (auto thread : py_threads) {
		thread->terminate();
	}
}

void PythonSocketServer::startServer(QString name) {

	if (!this->listen("toolkitpy.socket")) {
		std::cout << "Python Socket could not be started!\n" << std::endl;
	} else {
		std::cout << "Python Socket started listening on "
				  << this->fullServerName().toStdString() << "..." << std::endl;
	}
}

void PythonSocketServer::incomingConnection(quintptr socket_descriptor) {
	auto python_thread = new PythonThread(embedded_python, socket_descriptor, this);
	connect(python_thread, SIGNAL(finished()), python_thread, SLOT(deleteLater()));

	py_threads.push_back(python_thread);
	python_thread->start();
}

PythonLocalSocket::PythonLocalSocket(QObject *parent) : QLocalSocket(parent) {}

PythonLocalSocket::~PythonLocalSocket() {
	disconnect();
	deleteLater();
}

void PythonLocalSocket::py_write(std::string str) {
	this->write(str.data(), str.size());
}

std::string PythonLocalSocket::py_read(int len) {
	while(!this->waitForReadyRead()){}
	QByteArray data = this->read(len);
	return QString(data).toStdString();
}

std::string PythonLocalSocket::py_readline() {
	while(!this->waitForReadyRead()){}
	auto line = this->readLine(1024);
	return QString(line).toStdString();
}

void PythonLocalSocket::py_flush() {
	this->flush();
}

bool PythonLocalSocket::is_closed() {
	return !this->isOpen();
}
