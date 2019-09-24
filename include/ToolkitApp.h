#ifndef ToolkitApp_h_INCLUDED
#define ToolkitApp_h_INCLUDED

/*
 * A class that implements an extendable Application to render and work with RBDL Models
 */

#include <QMainWindow>
#include <QMenuBar>
#include <QDockWidget>
#include <QPluginLoader>
#include <QSettings>

#include "SceneWidget.h"
#include "rbdl_wrapper.h"
#include "Timeline.h"

class SceneWidget;

class ToolkitApp : public QMainWindow
{
	Q_OBJECT
	private:
		SceneWidget* main_display;
		ToolkitTimeline* timeline;

		QMenuBar* main_menu_bar;
		QMenu* toolkit_menu;
		QMenu* file_menu;
		QMenu* view_menu;
		QMenu* plugin_menu;

		std::vector<QDockWidget*> view_widgets;
		std::vector<RBDLModelWrapper*> loaded_models;

		//plugins that will always be loaded
		std::vector<QPluginLoader*> core_plugins;

		//plugins which can be selectively loaded
		std::vector<QPluginLoader*> availible_plugins;

		void initPlugins();
		void setPluginUsage(unsigned int plugin_ref, bool state);

	public:
		ToolkitApp(QWidget *parent = 0);
		std::vector<RBDLModelWrapper*>* getLoadedModels();

		void addView(QString name, QWidget *view_widget, Qt::DockWidgetArea area=Qt::RightDockWidgetArea, bool show_tilte = true);
		void deleteView(QString name);

		void addFileAction(QAction* file_action);

		ToolkitTimeline* getToolkitTimeline() { return timeline; }

	public slots:
		void action_reload_files();
		void action_load_model();

		void loadModel(const QString &model_file); 

	signals:
		void reload_files();
};


#endif

