#include "EmbeddedPython.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

const char *INIT_SCRIPT =
"print('Hello?')\n"
"\n"
"s = socket.socket()\n"
"s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)\n"
"s.bind(('0.0.0.0', 2000))\n"
"s.listen(1)\n"
"c = s.accept()[0] # client socket\n"
"\n"
"def sread(s, len):\n"
"        return s.recv(len)\n"
"def swrite(s, str):\n"
"        return s.sendall(str)\n"
"def sreadline(s):\n"
"        return sread(s, 256) # lazy implementation for quick testing\n"
"socket.socket.read = sread\n"
"socket.socket.write = swrite\n"
"socket.socket.readline = sreadline\n"
"sys.stdin = c\n"
"sys.stdout = c\n"
"sys.stderr = c\n"
"code.interact()\n";

EmbeddedPython::EmbeddedPython() {
	Py_Initialize();
	PyObject *pSocket = PyImport_Import(PyUnicode_FromString("socket"));
	PyObject *pCode = PyImport_Import(PyUnicode_FromString("code"));
	PyObject *pSys = PyImport_Import(PyUnicode_FromString("sys"));
	PyRun_SimpleString(INIT_SCRIPT);
	//PyRun_SimpleString("print('Embedded Python Environment initialized')");
}

EmbeddedPython::~EmbeddedPython() {
	Py_Finalize();
}
