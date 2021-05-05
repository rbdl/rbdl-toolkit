#include "PythonPlugin.h"
#include <iostream>
#include <unistd.h>

#include <QCommandLineParser>
#include <QCommandLineOption>

ToolkitTerminal::ToolkitTerminal(const QString socket_addr, QWidget *parent) : QTermWidget(false, parent) {
	socket = new QLocalSocket(this);

	connect(this, &ToolkitTerminal::sendData, [this](const char *data, int size){
		if ( strcmp(data, "\r") == 0 ) {
			write(this->getPtySlaveFd(), data, size);
			write(this->getPtySlaveFd(), "\n", 1);
			this->socket->write(data, size);
			this->socket->write("\n", 1);
		} else {
			write(this->getPtySlaveFd(), data, size);
			this->socket->write(data, size);
		}
	});

	connect(socket, &QLocalSocket::readyRead, [this](){
		QByteArray data = socket->readAll();
		write(this->getPtySlaveFd(), data.data(), data.size());
	});
	connect(socket, &QLocalSocket::errorOccurred, this , &ToolkitTerminal::atError);

	this->startTerminalTeletype();

	socket->connectToServer(socket_addr);
}

ToolkitTerminal::~ToolkitTerminal() {
	socket->close();
}

void ToolkitTerminal::atError(QLocalSocket::LocalSocketError err) {
	std::cout << socket->errorString().toStdString() << std::endl;
}

PythonPlugin::PythonPlugin() {
	parentApp = NULL;

	embedded_python = NULL;
	python_gateway = NULL;
	console = NULL;
}

PythonPlugin::~PythonPlugin() {
	if (python_gateway != NULL) {
		python_gateway->close();
		delete python_gateway;
	}

	if (console != NULL) {
		parentApp->deleteView("ToolkitTerminal");
	}

	if (embedded_python != NULL) {
		delete embedded_python;
	}
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

	embedded_python = new EmbeddedPython(parentApp);
	connect(this, &PythonPlugin::socket_client_connected,
			embedded_python, &EmbeddedPython::connect_client);

	python_gateway = new QLocalServer();
	python_gateway->listen("toolkitpy.socket");
	std::cout << python_gateway->fullServerName().toStdString() << std::endl;
	connect(python_gateway, &QLocalServer::newConnection, this, &PythonPlugin::handleGatewayConnection);

	console = new ToolkitTerminal(python_gateway->fullServerName());
	QFont font = QApplication::font();
#ifdef Q_OS_MACOS
    font.setFamily(QStringLiteral("Monaco"));
#elif defined(Q_WS_QWS)
    font.setFamily(QStringLiteral("fixed"));
#else
    font.setFamily(QStringLiteral("Monospace"));
#endif
	font.setPointSize(11);

    console->setTerminalFont(font);

	parentApp->addView("ToolkitTerminal", qobject_cast<QWidget*>(console), Qt::BottomDockWidgetArea);
}

void PythonPlugin::handleGatewayConnection() {
	QLocalSocket* socket = python_gateway->nextPendingConnection();
    connect(socket, &QLocalSocket::disconnected, [socket](){
       socket->disconnect();
       socket->deleteLater();
    });

	emit socket_client_connected(socket);
}
