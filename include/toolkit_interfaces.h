#ifndef TOOLKIT_INTERFACES_H_INCLUDED
#define TOOLKIT_INTERFACES_H_INCLUDED

#include <QString>
#include <QWidget>
#include <QtPlugin>

#include "ToolkitApp.h"

class CoreInterface {
	public:
		virtual ~CoreInterface() {}

		virtual void init(ToolkitApp* app) = 0;
};

#define CoreInterface_iid "org.orb-hd.rbdl-toolkit.CorePlugin"
Q_DECLARE_INTERFACE(CoreInterface, CoreInterface_iid)


class OptionalInterface{
	public:
		virtual ~OptionalInterface() {}

		virtual void init(ToolkitApp* app) = 0;
};

#define OptionalInterface_iid "org.orb-hd.rbdl-toolkit.OptionalPlugin"
Q_DECLARE_INTERFACE(OptionalInterface, OptionalInterface_iid)



#endif 

