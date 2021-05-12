#include "EmbeddedPython.h"

#include "PythonToolkitInterface.h"
#include "PythonSocket.h"

/*==============================================================================
 * EmbeddedPython
 *=============================================================================*/

class EmbeddedPythonImpl {
	private:
		pybind11::scoped_interpreter* python_guard;
		pybind11::dict *locals;

		ToolkitApp* toolkit;
	public:
		EmbeddedPythonImpl(ToolkitApp* toolkit);
		~EmbeddedPythonImpl();

		void run_code(QString py_code, PythonLocalSocket *s);
		void run_source(QString py_file);
};

EmbeddedPythonImpl::EmbeddedPythonImpl(ToolkitApp* toolkit) : toolkit(toolkit) {
	python_guard = new pybind11::scoped_interpreter();
	pybind11::module::import("pythonlocalsocket");
	using namespace pybind11::literals;

	pybind11::module_ sys = pybind11::module_::import("sys");
	pybind11::module_ traceback = pybind11::module_::import("traceback");
	locals = new pybind11::dict(
		"sys"_a = sys,
		"traceback"_a = traceback
	);
	pybind11::eval<pybind11::eval_statements>(INIT_SCRIPT, *locals);
}

EmbeddedPythonImpl::~EmbeddedPythonImpl() {
	delete python_guard;
}

void EmbeddedPythonImpl::run_code(QString py_code, PythonLocalSocket *s) {
	pybind11::object pls = pybind11::cast(s);
	(*locals)["pls"] = pls;

}


EmbeddedPython::EmbeddedPython(ToolkitApp* toolkit) {
	impl = new EmbeddedPythonImpl(toolkit);
}

EmbeddedPython::~EmbeddedPython() {
	delete impl;
}

void EmbeddedPython::run_code(QString py_code, PythonLocalSocket* callback_socket) {
	impl->run_code(py_code, callback_socket);
}
