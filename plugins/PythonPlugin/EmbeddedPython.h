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

	public Q_SLOTS:
		void connect_client(QLocalSocket*);
	private:
		ToolkitApp *toolkit;

		void* guard;
};

#endif
