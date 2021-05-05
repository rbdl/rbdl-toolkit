#ifndef EMBEDDED_PYTHON_H
#define EMBEDDED_PYTHON_H

#include <QObject>
#include <ToolkitApp.h>
#include <QLocalSocket>

class EmbeddedPython : public QObject {
	Q_OBJECT

	public:
		EmbeddedPython(ToolkitApp* toolkit);
		~EmbeddedPython();

		void startPythonShell(QLocalSocket*);
	private:
		ToolkitApp *toolkit;
};

#endif
