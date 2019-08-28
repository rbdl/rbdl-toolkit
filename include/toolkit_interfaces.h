#ifndef TOOLKIT_INTERFACES_H_INCLUDED
#define TOOLKIT_INTERFACES_H_INCLUDED

#include <QString>
#include <QWidget>
#include <QtPlugin>

#include "ToolkitApp.h"

class ViewInterface {
	public:
		virtual ~ViewInterface() {}

		virtual QString getViewName() = 0;
		virtual QWidget* getViewWidget() = 0;
};

#define ViewInterface_iid "org.orb-hd.rbdl-toolkit.ViewPlugin"
Q_DECLARE_INTERFACE(ViewInterface, ViewInterface_iid)

class ModelInterface {
	public:
		virtual ~ModelInterface() {}
};

#define ModelInterface_iid "org.orb-hd.rbdl-toolkit.ModelPlugin"
Q_DECLARE_INTERFACE(ModelInterface, ModelInterface_iid)

class FileInterface {
	public:
		virtual ~FileInterface() {}

		virtual QString getFilePurpos() = 0;
		virtual void loadFile(ToolkitApp* app, QString path) = 0;

};

#define FileInterface_iid "org.orb-hd.rbdl-toolkit.FilePlugin"
Q_DECLARE_INTERFACE(FileInterface, FileInterface_iid)



#endif 

