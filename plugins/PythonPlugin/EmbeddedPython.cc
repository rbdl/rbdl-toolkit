#include "EmbeddedPython.h"

#include "PythonToolkitInterface.h"
#include "PythonSocket.h"

/*==============================================================================
 * PythonThread
 *=============================================================================*/

PythonThread::PythonThread(EmbeddedPython* ep, quintptr socket_descriptor, QObject* parent)
	: QThread(parent)
{
	this->socket_descriptor = socket_descriptor;
	this->embedded_python = ep;
}

void PythonThread::run() {
	socket = new PythonLocalSocket();

	if (!socket->setSocketDescriptor(this->socket_descriptor)) {
		return;
	}

	connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

	embedded_python->startPythonShell(socket);
}

void PythonThread::disconnected() {
	socket->deleteLater();
	exit(0);
}

/*==============================================================================
 * EmbeddedPython
 *=============================================================================*/

EmbeddedPython::EmbeddedPython(ToolkitApp* toolkit) : toolkit(toolkit) {
}

EmbeddedPython::~EmbeddedPython() {
}

void EmbeddedPython::startPythonShell(PythonLocalSocket *s) {
	auto guard = new pybind11::scoped_interpreter();
	pybind11::module::import("pythonlocalsocket");
	using namespace pybind11::literals;

	pybind11::object pls = pybind11::cast(s);
	pybind11::module_ sys = pybind11::module_::import("sys");
	pybind11::module_ traceback = pybind11::module_::import("traceback");
	auto locals = pybind11::dict(
		"pls"_a = pls,
		"sys"_a = sys,
		"traceback"_a = traceback
	);
	pybind11::exec(INIT_SCRIPT, locals);
	delete guard;
}
