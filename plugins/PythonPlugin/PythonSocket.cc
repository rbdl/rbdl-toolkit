#include "PythonSocket.h"

/*==============================================================================
 * PythonSocketServer
 *=============================================================================*/

PythonSocketServer::PythonSocketServer(EmbeddedPython* ep, QObject* parent)
	: QLocalServer(parent), embedded_python(ep) {}

PythonSocketServer::~PythonSocketServer() {
	for (auto socket : py_sockets) {
		socket->disconnect();
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
	auto socket = new PythonLocalSocket();

	if (!socket->setSocketDescriptor(socket_descriptor)) {
		return;
	}
	py_sockets.push_back(socket);
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
	while(!this->waitForReadyRead() && !this->canReadLine()){}
	auto line = this->readLine(1024);
	std::cout << line.toStdString();
	return QString(line).toStdString();
}

void PythonLocalSocket::py_flush() {
	this->flush();
}

bool PythonLocalSocket::is_closed() {
	return !this->isOpen();
}
