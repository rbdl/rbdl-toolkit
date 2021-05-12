#include "PythonPlugin.h"
#include <iostream>
#include <unistd.h>

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QThread>

/*==============================================================================
 * ToolkitSocketTerminal
 *=============================================================================*/

ToolkitSocketTerminal::ToolkitSocketTerminal(const QString socket_addr, QWidget *parent)
	: QTermWidget(false, parent)
 {
	socket = new QLocalSocket(this);

	connect(this, &ToolkitSocketTerminal::sendData, [this](const char *data, int size){
		this->socket->write(data, size);
	});

	connect(socket, &QLocalSocket::readyRead, [this](){
		QByteArray data = socket->readAll();
		write(this->getPtySlaveFd(), data.data(), data.size());
	});
	connect(socket, &QLocalSocket::errorOccurred, this , &ToolkitSocketTerminal::atError);

	this->startTerminalTeletype();

	socket->connectToServer(socket_addr);
}

ToolkitSocketTerminal::~ToolkitSocketTerminal() {
	socket->close();
}

void ToolkitSocketTerminal::atError(QLocalSocket::LocalSocketError err) {
	std::cout << socket->errorString().toStdString() << std::endl;
}

/*==============================================================================
 *PythonPlugin
 *=============================================================================*/

PythonPlugin::PythonPlugin() {
	parentApp = NULL;

	embedded_python = NULL;
	python_server = NULL;
	console = NULL;
}

PythonPlugin::~PythonPlugin() {
	if (python_server != NULL) {
		python_server->close();
		delete python_server;
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
	python_server = new PythonSocketServer(embedded_python);
	python_server->startServer("toolkitpy.socket");

	console = new ToolkitSocketTerminal(python_server->fullServerName());
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
	//console->setShellProgram(QString("nc"));
	//QStringList args;
	//args << QString("-U") << python_server->fullServerName();
	//console->setArgs(args);
	//console->startShellProgram();

	parentApp->addView("ToolkitTerminal", qobject_cast<QWidget*>(console), Qt::BottomDockWidgetArea);
}

