#include "PythonSocket.h"

/*==============================================================================
 * PythonSocketServer
 *=============================================================================*/

PythonSocketServer::PythonSocketServer(EmbeddedPython* ep, QObject* parent)
	: QLocalServer(parent), embedded_python(ep) {}

PythonSocketServer::~PythonSocketServer() {
	for (auto socket : py_sockets) {
		delete socket;
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
	connect(socket, &PythonLocalSocket::received_line, this, &PythonSocketServer::run_code);

	embedded_python->init_shell(socket);
	py_sockets.push_back(socket);
}

void PythonSocketServer::run_code(QString line, PythonLocalSocket* s) {
	embedded_python->run_code(line, s);
}

PythonLocalSocket::PythonLocalSocket(QObject *parent) : QLocalSocket(parent) {
	connect(this, &QLocalSocket::readyRead, this, &PythonLocalSocket::recv_data);
}

PythonLocalSocket::~PythonLocalSocket() {
	disconnect();
	deleteLater();
}

void PythonLocalSocket::recv_data() {
	if (this->canReadLine()) {
		auto line = this->readLine(1024);
		emit received_line(QString(line), this);
	}
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
	while(!this->waitForReadyRead() {}
	auto line = this->readLine(1024);
	std::cout << line.toStdString();
	return QString(line).toStdString();
}

void PythonLocalSocket::py_flush() {
	this->flush();
}

int PythonLocalSocket::py_id() {
	return this->socketDescriptor();
}

bool PythonLocalSocket::is_closed() {
	return !this->isOpen();
}
