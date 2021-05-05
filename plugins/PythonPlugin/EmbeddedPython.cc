#include "EmbeddedPython.h"

#include <pybind11/embed.h>

const char *INIT_SCRIPT =
"import code\n"
//"\n"
//"s = socket.socket()\n"
//"s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)\n"
//"s.bind(('0.0.0.0', 2000))\n"
//"s.listen(1)\n"
//"c = s.accept()[0] # client socket\n"
//"\n"
//"def sread(s, len):\n"
//"        return s.recv(len)\n"
//"def swrite(s, str):\n"
//"        return s.sendall(str)\n"
//"def sreadline(s):\n"
//"        return sread(s, 256) # lazy implementation for quick testing\n"
//"socket.socket.read = sread\n"
//"socket.socket.write = swrite\n"
//"socket.socket.readline = sreadline\n"
//"sys.stdin = c\n"
//"sys.stdout = c\n"
//"sys.stderr = c\n"
"code.interact()\n";

class ToolkitSocket {
	private:
		QLocalSocket *s;
		ToolkitSocket(QLocalSocket* s) : s(s) {}
	public:
		static ToolkitSocket create(QLocalSocket* s) {
			return ToolkitSocket(s);
		}

		void send(std::string str) {
			s->write(str.data(), str.size());
		}
};

PYBIND11_EMBEDDED_MODULE(toolkitsocket, m) {
	py::class_<ToolkitSocket>(m, "ToolkitSocket")
		.def(py::init(&ToolkitSocket::create))
		.def("send", &ToolkitSocket::send);
}

EmbeddedPython::EmbeddedPython(ToolkitApp* toolkit) : toolkit(toolkit) {
	guard = (void*)new pybind11::scoped_interpreter();
	pybind11::module_ sys = pybind11::module_::import("sys");
	pybind11::module_ code = pybind11::module_::import("code");

	pybind11::exec(INIT_SCRIPT);
}

EmbeddedPython::~EmbeddedPython() {
	delete (pybind11::scoped_interpreter*)guard;
}

void EmbeddedPython::connect_client(QLocalSocket* socket) {
	pybind11::module_ code = pybind11::module_::import("toolkitsocket");

}
