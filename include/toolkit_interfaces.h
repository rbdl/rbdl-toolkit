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


class CoreInterface {
	public:
		virtual ~CoreInterface() {}

		virtual void init(ToolkitApp* app) = 0;
};

#define CoreInterface_iid "org.orb-hd.rbdl-toolkit.CorePlugin"
Q_DECLARE_INTERFACE(CoreInterface, CoreInterface_iid)



#endif 

