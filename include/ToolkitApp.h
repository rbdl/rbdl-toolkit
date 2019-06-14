#ifndef ToolkitApp_h_INCLUDED
#define ToolkitApp_h_INCLUDED

/*
 * A class that implements an extendable Application to render and work with RBDL Models
 */

#include <QMainWindow>
#include <QMenuBar>
#include <QDockWidget>

#include "SceneWidget.h"
#include "rbdl_wrapper.h"

class SceneWidget;

class ToolkitApp : public QMainWindow
{
	Q_OBJECT
	private:
		SceneWidget* main_display;

		QMenuBar* main_menu_bar;
		QMenu* file_menu;
		QMenu* view_menu;

		std::vector<QDockWidget*> view_widgets;
		std::vector<RBDLModelWrapper*> loaded_models;

	public:
		ToolkitApp(QWidget *parent = 0);
		void addView(QString name, QWidget *view_widget, Qt::DockWidgetArea area=Qt::RightDockWidgetArea);

	public slots:
		void action_reload_files();
		void loadModel(const QString &model_file); 
		void action_load_model();

};


#endif

