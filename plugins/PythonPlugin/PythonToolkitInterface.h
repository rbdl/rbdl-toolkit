#ifndef PYTHON_TOOLKIT_INTERFACE_INCLUDED
#define PYTHON_TOOLKIT_INTERFACE_INCLUDED

#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <PythonSocket.h>

const char *INIT_SCRIPT =
    "from codeop import CommandCompiler, compile_command\n"
    "class ToolkitPythonSocketExecutor():\n"
    "  def __init__(self, sock, locals=None, filename='console'):\n"
    "    if locals is None:\n"
    "      locals = {\"__name__\": \"__console__\", \"__doc__\": None}\n"
    "    self.locals = locals\n"
    "    self.compile = CommandCompiler()\n"
    "    self.filename = filename\n"
    "    self.resetbuffer()\n"
    "    self.sock = sock\n"
    "    try:\n"
    "        sys.ps1\n"
    "    except AttributeError:\n"
    "        sys.ps1 = \">>> \"\n"
    "    try:\n"
    "        sys.ps2\n"
    "    except AttributeError:\n"
    "        sys.ps2 = \"... \"\n"
    "    self.more = 0\n"

    "  def runsource(self, source, filename=\"<input>\", symbol=\"single\"):\n"
    "    try:\n"
    "        code = self.compile(source, filename, symbol)\n"
    "    except (OverflowError, SyntaxError, ValueError):\n"
    "        # Case 1\n"
    "        self.showsyntaxerror(filename)\n"
    "        return False\n"
    "    if code is None:\n"
    "      # Case 2\n"
    "      return True\n"
    "    # Case 3\n"
    "    self.runcode(code)\n"
    "    return False\n"

    "  def runcode(self, code):\n"
    "    try:\n"
    "        exec(code, self.locals)\n"
    "    except SystemExit:\n"
    "        raise\n"
    "    except:\n"
    "        self.showtraceback()\n"

    "  def showsyntaxerror(self, filename=None):\n"
    "    type, value, tb = sys.exc_info()\n"
    "    sys.last_type = type\n"
    "    sys.last_value = value\n"
    "    sys.last_traceback = tb\n"
    "    if filename and type is SyntaxError:\n"
    "        # Work hard to stuff the correct filename in the exception\n"
    "        try:\n"
    "            msg, (dummy_filename, lineno, offset, line) = value.args\n"
    "        except ValueError:\n"
    "            # Not the format we expect; leave it alone\n"
    "            pass\n"
    "        else:\n"
    "            # Stuff in the right filename\n"
    "            value = SyntaxError(msg, (filename, lineno, offset, line))\n"
    "            sys.last_value = value\n"
    "    if sys.excepthook is sys.__excepthook__:\n"
    "        lines = traceback.format_exception_only(type, value)\n"
    "        self.write(''.join(lines))\n"
    "    else:\n"
    "        # If someone has set sys.excepthook, we let that take precedence\n"
    "        # over self.write\n"
    "        sys.excepthook(type, value, tb)\n"

    "  def showtraceback(self):\n"
    "    sys.last_type, sys.last_value, last_tb = ei = sys.exc_info()\n"
    "    sys.last_traceback = last_tb\n"
    "    try:\n"
    "        lines = traceback.format_exception(ei[0], ei[1], last_tb.tb_next)\n"
    "        if sys.excepthook is sys.__excepthook__:\n"
    "            self.write(''.join(lines))\n"
    "        else:\n"
    "            # If someone has set sys.excepthook, we let that take precedence\n"
    "            # over self.write\n"
    "            sys.excepthook(ei[0], ei[1], last_tb)\n"
    "    finally:\n"
    "        last_tb = ei = None\n"

    "  def showbanner(self, banner=None):\n"
    "    cprt = 'Type \"help\", \"copyright\", \"credits\" or \"license\" for more information.'\n"
    "    if banner is None:\n"
    "        self.write('Python %s on %s\\n%s\\n(%s)\\n' % (sys.version, sys.platform, cprt, \"ToolkitPythonEnvironment\"))\n"
    "    elif banner:\n"
    "        self.write(\"%s\\n\" % str(banner))\n"
    "    self.show_prompt()\n"

    "  def show_prompt(self):\n"
    "    if self.more:\n"
    "       prompt = sys.ps2\n"
    "    else:\n"
    "       prompt = sys.ps1\n"
    "    self.write(prompt)\n"

    "  def interact_line(self, line):\n"
    "    sys.stdout = self.sock\n"
    "    try:\n"
    "      self.push(line)\n"
    "    except KeyboardInterrupt:\n"
    "      self.write(\"\\nKeyboardInterrupt\\n\")\n"
    "      self.resetbuffer()\n"
    "      more = 0\n"
    "    self.show_prompt()\n"

    "  def push(self, line):\n"
    "    self.buffer.append(line)\n"
    "    source = \"\\n\".join(self.buffer)\n"
    "    more = self.runsource(source, self.filename)\n"
    "    if not more:\n"
    "        self.resetbuffer()\n"
    "    self.more = more\n"

    "  def resetbuffer(self):\n"
    "    self.buffer = []\n"

    "  def write(self, data):\n"
    "    if not self.sock.is_closed():\n"
    "      self.sock.write(data.encode('ascii'))\n"
    "      self.sock.flush()\n"

    "  def raw_input(self, prompt='>>>'):\n"
    "    if (self.sock.is_closed()):\n"
    "      raise EOFError('Socket closed')\n"

    "    self.write(prompt)\n"
    "    r = self.sock.readline()\n"
    "    return r\n"

    "python_shells = {}";

const char* INIT_SHELL =
    "tpse = ToolkitPythonSocketExecutor(pls)\n"
    "tpse.showbanner()\n"
    "python_shells[pls.id()] = tpse \n";

const char* RUN_CMD =
    "python_shells[pls.id()].interact_line(code_torun)\n";

PYBIND11_EMBEDDED_MODULE(pythonlocalsocket, m) {
	pybind11::class_<PythonLocalSocket>(m, "PythonLocalSocket")
		.def(pybind11::init<QObject*>())
		.def("write", &PythonLocalSocket::py_write)
		.def("read", &PythonLocalSocket::py_read)
		.def("readline", &PythonLocalSocket::py_readline)
		.def("is_closed", &PythonLocalSocket::is_closed)
		.def("flush", &PythonLocalSocket::py_flush)
        .def("id", &PythonLocalSocket::py_id);
}

#endif
