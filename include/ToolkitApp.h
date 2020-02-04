#ifndef ToolkitApp_h_INCLUDED
#define ToolkitApp_h_INCLUDED

/*
 * A class that implements an extendable Application to render and work with RBDL Models
 */

#include <functional>

#include <QMainWindow>
#include <QMenuBar>
#include <QDockWidget>
#include <QPluginLoader>
#include <QSettings>
#include <QCommandLineParser>

#include "SceneWidget.h"
#include "rbdl_wrapper.h"
#include "Timeline.h"
#include "ModelSelectorDialog.h"

class SceneWidget;

class ToolkitApp : public QMainWindow
{
	Q_OBJECT
	private:
		SceneWidget* main_display;
		ToolkitTimeline* timeline;
		QCommandLineParser cmd_parser;

		QMenuBar* main_menu_bar;
		QMenu* toolkit_menu;
		QMenu* file_menu;
		QMenu* view_menu;
		QMenu* plugin_menu;

		std::vector<QDockWidget*> view_widgets;
		std::vector<RBDLModelWrapper*> loaded_models;

		std::vector<std::function<void(QCommandLineParser&)>> cmd_hooks;
		std::map<QString, QPluginLoader*> toolkit_plugins;

		void initPlugins();
		void setPluginUsage(QString plugin_name, bool state);
		QVariant getPluginLoadSetting(QString plugin_name);
		void setPluginLoadSetting(QString plugin_name, bool load);

	public:
		QSettings toolkit_settings;

		ToolkitApp(QWidget *parent = 0);
		std::vector<RBDLModelWrapper*>* getLoadedModels();

		RBDLModelWrapper* selectModel(ModelFilter filter);

		void addView(QString name, QWidget *view_widget, Qt::DockWidgetArea area=Qt::RightDockWidgetArea, bool show_tilte = true);
		void deleteView(QString name);

		void addFileAction(QAction* file_action);

		void addMenu(QMenu* menu);

		void addCmdOption(QCommandLineOption &option, std::function<void(QCommandLineParser&)>);
		void parseCmd(QApplication& app);

		void showWarningDialog(QString warning_msg);

		static void showExceptionDialog(std::exception& e);

		ToolkitTimeline* getToolkitTimeline() { return timeline; }
		SceneWidget* getSceneObj() { return main_display; }

	public slots:
		void action_reload_files();
		void action_load_model();

		void model_visual_update(Qt3DCore::QEntity* visual);

		void loadModel(const QString &model_file); 

	signals:
		void reload_files();

};


#endif

