#ifndef ToolkitApp_h_INCLUDED
#define ToolkitApp_h_INCLUDED

#include <QMainWindow>

/*
 * A class that implements an extendable Application to render and work with RBDL Models
 */

#include "SceneWidget.h"

class ToolkitApp : public QMainWindow
{
	Q_OBJECT
	private:
		SceneWidget* main_display;
	public:
		ToolkitApp(QWidget *parent = 0);

	public slots:

};


#endif

