#ifndef TESTPLUGIN_H_INCLUDED
#define TESTPLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>

#include "toolkit_interfaces.h"

class TestPlugin : public QObject, public ViewInterface {
	Q_OBJECT
	Q_PLUGIN_METADATA(IID ViewInterface_iid FILE "metadata.json")
	Q_INTERFACES(ViewInterface)

	public:
		TestPlugin();

		QString getViewName();
		QWidget* getViewWidget();

	private:
		QWidget *view;
		QString view_name;
};

#endif 

