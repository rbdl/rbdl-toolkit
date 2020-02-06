#include "ToolkitApp.h"
#include "render_util.h"
#include "util.h"
#include "rbdl/rbdl_errors.h"
#include "toolkit_interfaces.h"
#include "toolkit_errors.h"

#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>
#include <QKeySequence>
#include <QMessageBox>
#include <Qt3DCore>
#include <QCoreApplication>
#include <QCommandLineOption>

ToolkitApp::ToolkitApp(QWidget *parent) {
	//setup standard menu
	main_menu_bar = new QMenuBar(NULL);
	toolkit_menu = main_menu_bar->addMenu("Toolkit");
	file_menu = toolkit_menu->addMenu("File");
	view_menu = main_menu_bar->addMenu("Views");
	plugin_menu = main_menu_bar->addMenu("Plugins");

	file_menu->addAction("Load Model", this, "aaction_load_model()");
	toolkit_menu->addAction("Reload Files", this, "aaction_reload_files()", QKeySequence::fromString("F5"));
	this->setMenuBar(main_menu_bar);

	//main view
	main_display = new SceneWidget(this);
	this->setCentralWidget(main_display);

	//setup parser
	cmd_parser.setApplicationDescription("Application for visualizing and working with rbdl models");
	cmd_parser.addHelpOption();
	cmd_parser.addVersionOption();

	QCommandLineOption model_option( QStringList() << "m" << "model",
	                                 "Load lua model files <file>", 
	                                 "file"
	                               );
	QCommandLineOption plugin_option( QStringList() << "p" << "plugin",
	                                 "Load additional plugins <plugin name/path>", 
	                                 "plugin"
	                               );
	addCmdOption(model_option, [this](QCommandLineParser& parser) {
		//load models
		auto model_list = parser.values("model");
		for ( auto m : model_list ) {
			this->loadModel(findFile(m));
		}
	});
	addCmdOption(plugin_option, [this](QCommandLineParser& parser) {
		//load models
		auto plugin_list = parser.values("plugin");
		for ( auto p : plugin_list) {
			try {
				QString plugin_path = findPlugin(p);
				QPluginLoader* loader = new QPluginLoader(plugin_path);
				QString plugin_iid = loader->metaData().value("IID").toString();
				QString plugin_name = loader->metaData().value("MetaData").toObject().value("Name").toString();

				//check if plugin was already loaded
				if (toolkit_plugins.find(plugin_name) == toolkit_plugins.end()) {
					toolkit_plugins[plugin_name] = loader;
				} else {
					//plugin with same name was already found so skip this duplicate
					delete loader;
					continue;
				}
			} catch (RBDLToolkitError& e) {
				showWarningDialog(e.what());
				continue;
			}
		}
	});

	//timeline widget
	timeline = new ToolkitTimeline(this);
	addView("Timeline", timeline, Qt::BottomDockWidgetArea, false);
	connect(main_display, SIGNAL(frame_sync_signal(float)), timeline, SLOT(tick(float)));

	//add standard scene objects
	main_display->addSceneObject(createGridFloor(-15., 15., 32));
	resize(800, 600);

	//set standard search paths
	bool install_path_in_standart_list = false;
	QDir::addSearchPath("", "meshes/");
	auto paths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
	for (int i=0; i<paths.size(); i++) {
		QFileInfo check_file(paths.at(i));
		if (check_file.isDir()) {
			QDir dir(paths.at(i));
			QDir::addSearchPath("", dir.path());
			if (dir.cd("meshes")) {
				QDir::addSearchPath("", dir.path());
			}
			QDir dir1(paths.at(i));
			if (dir1.cd("plugins")) {
				QCoreApplication::addLibraryPath(dir1.path());
				QDir::addSearchPath("plugins", dir1.path());
			}
		}
		//check if the install dir is one of the standart search paths of qt
		if (paths.at(i).toStdString() == INSTALL_DIR) {
			install_path_in_standart_list = true;
		}
		//make sure install path gets searched
		if (i==paths.size()-1 && !install_path_in_standart_list) {
			paths << INSTALL_DIR;
		}
	}

	initPlugins();
}

void ToolkitApp::action_reload_files() {
	for (auto model : loaded_models) {
		Qt3DCore::QEntity*  model_render_obj = model->getRenderObj();
		main_display->removeSceneObject(model_render_obj);
		model->reload();

		model_render_obj = model->getRenderObj();
		main_display->addSceneObject(model_render_obj);
	}
}

void ToolkitApp::action_load_model() {
	QFileDialog file_dialog (this, "Select Model File");

	file_dialog.setNameFilter(tr("MeshupModels (*lua)"));
	file_dialog.setFileMode(QFileDialog::ExistingFile);

	if (file_dialog.exec()) {
		loadModel (file_dialog.selectedFiles().at(0));
	}	
}

std::vector<RBDLModelWrapper*>* ToolkitApp::getLoadedModels() {
	return &loaded_models;
}


void ToolkitApp::loadModel(const QString &model_file) {
	RBDLModelWrapper *model = new RBDLModelWrapper();
	bool errors_happend = false;

	Qt3DCore::QEntity* model_scene_obj;

	try {
		model_scene_obj = model->loadFromFile(model_file);
	} catch (RigidBodyDynamics::Errors::RBDLFileParseError& e) {
		errors_happend = true;
		QMessageBox errorBox;
		errorBox.setText(e.what());
		errorBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
		errorBox.setDefaultButton(QMessageBox::Cancel);
		int ret = errorBox.exec();
		switch(ret) {
			case QMessageBox::Retry:
				loadModel(model_file);
				break;
		}
	} catch (std::exception& e) {
		errors_happend = true;
		QMessageBox errorBox;
		errorBox.setText(e.what());
		errorBox.setStandardButtons(QMessageBox::Cancel);
		errorBox.setDefaultButton(QMessageBox::Cancel);
		errorBox.exec();
	} 

	if (!errors_happend) {
		//add group to scene object to toggle single models in render settings
		model_scene_obj->setProperty("Scene.ObjGroup", 
		                              QVariant(QString("%1: %2").arg(
		                                         QString::number(loaded_models.size()), 
		                                         model->getFileName()
		                                       )
		                             ));
		loaded_models.push_back(model);
		main_display->addSceneObject(model_scene_obj);

		connect(timeline, SIGNAL(timeChanged(float)), model, SLOT(model_update(float)));

		connect(model, SIGNAL(visual_added(Qt3DCore::QEntity*)), this, SLOT(model_visual_update(Qt3DCore::QEntity*)));
	}
}

void ToolkitApp::model_visual_update(Qt3DCore::QEntity* visual) {
	QVariant obj_grouping = visual->property("Scene.ObjGroup");
	if (obj_grouping.isValid()) {
		QString group_name = obj_grouping.toString();
		this->getSceneObj()->addSceneObjectToGroup(visual, group_name);
	}
}

void ToolkitApp::showExceptionDialog(std::exception& e) {
	QMessageBox errorBox;
	errorBox.setText(e.what());
	errorBox.setStandardButtons(QMessageBox::Cancel);
	errorBox.setDefaultButton(QMessageBox::Cancel);
	errorBox.exec();
}



void ToolkitApp::addView(QString name, QWidget *view_widget, Qt::DockWidgetArea area, bool show_tilte) {
	QDockWidget* dock = new QDockWidget(name, this);
	dock->setWidget(view_widget);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	if (!show_tilte) {
		dock->setTitleBarWidget(new QWidget());
	}

	addDockWidget(area, dock);

	view_widgets.push_back(dock);
	view_menu->addAction(dock->toggleViewAction());
}

void ToolkitApp::deleteView(QString name) {
	for(auto it = view_widgets.begin(); it != view_widgets.end(); it++) {
		if ((*it)->windowTitle() == name) {
			delete (*it);
			view_widgets.erase(it);
			break;
		}
	}
}

QVariant ToolkitApp::getPluginLoadSetting(QString plugin_name) {
	QString settings_key = QString("plugins.load.%1").arg(plugin_name);
	return toolkit_settings.value(settings_key);
}

void ToolkitApp::setPluginLoadSetting(QString plugin_name, bool load) {
	QString settings_key = QString("plugins.load.%1").arg(plugin_name);
	toolkit_settings.setValue(settings_key, load);
}

void ToolkitApp::initPlugins() {
	auto plugins = findAllPlugins();

	foreach (const QString plugin_path, plugins) {
		QPluginLoader* loader = new QPluginLoader(plugin_path);
		QString plugin_iid = loader->metaData().value("IID").toString();
		QString plugin_name = loader->metaData().value("MetaData").toObject().value("Name").toString();

		//check if plugin was already loaded
		if (toolkit_plugins.find(plugin_name) == toolkit_plugins.end()) {
			toolkit_plugins[plugin_name] = loader;
		} else {
			//plugin with same name was already found so skip this duplicate
			continue;
		}

		//core plugins will be loaded directly if not disabled in settings
		if (plugin_iid == CoreInterface_iid) {
			if (getPluginLoadSetting(plugin_name).isNull()) {
				//core plugins are loaded per default
				setPluginLoadSetting(plugin_name, true);
			} 
			if ( getPluginLoadSetting(plugin_name).toBool() == true) {
				QObject* obj = loader->instance();
				if (obj) {
					CoreInterface* instance = qobject_cast<CoreInterface*>(obj); 
					instance->init(this);
				} else {
					std::cout << "Loading core plugin " << loader->fileName().toStdString() <<" failed!" << std::endl;
					std::cout << loader->errorString().toStdString() << std::endl;
				}
			}

		//All other plugins will be toggled via the plugins menu
		} else {
			//create menu action to enable and disable plugins
			QAction *plugin_action = plugin_menu->addAction(plugin_name); 
			plugin_action->setCheckable(true);

			//connect to action via lambda function
			connect(plugin_action, &QAction::changed, [=] 
			{ 
				setPluginUsage(plugin_name, plugin_action->isChecked()); 
			});

			if (getPluginLoadSetting(plugin_name).isNull()) {
				//optional plugins only get loaded if user enables them
				setPluginLoadSetting(plugin_name, false);
			} else if ( getPluginLoadSetting(plugin_name).toBool() == true) {
				plugin_action->setChecked(true);
			}
		}
	}
}

void ToolkitApp::setPluginUsage(QString plugin_name, bool state) {

	QPluginLoader* loader = toolkit_plugins[plugin_name];
	QString plugin_iid = loader->metaData().value("IID").toString();
	setPluginLoadSetting(plugin_name, state);

	if (state) {
		//load plugin
		if (loader->load()) {
			QObject* obj = loader->instance();
			if (obj) {
				OptionalInterface* instance = qobject_cast<OptionalInterface*>(obj); 
				instance->init(this);
			}
		} else {
			std::cout << "Loading plugin " << loader->fileName().toStdString() <<" failed!" << std::endl;
			std::cout << loader->errorString().toStdString() << std::endl;
		}
	} else {
		//unload plugin
		loader->unload();
	}
}

void ToolkitApp::addFileAction(QAction* action) {
	file_menu->addAction(action);
}

void ToolkitApp::parseCmd(QApplication& app) {
	cmd_parser.process(app);

	//execute all cmd hooks to do all actions specified by the options
	for (int i=0; i<cmd_hooks.size(); i++) {
		cmd_hooks[i](cmd_parser);
	}
}

RBDLModelWrapper* ToolkitApp::selectModel(ModelFilter filter) {
	ModelSelectorDialog select_dialog(&loaded_models, filter, this);

	if(select_dialog.exec()) {
		return select_dialog.getSelectetModel();
	}

	//no model was selected
	return nullptr;
}

void ToolkitApp::addCmdOption(QCommandLineOption &option, std::function<void(QCommandLineParser&)> option_logic) {
	cmd_parser.addOption(option);

	cmd_hooks.push_back(option_logic);
}

void ToolkitApp::addMenu(QMenu* menu) {
	main_menu_bar->addMenu(menu);
}

void ToolkitApp::showWarningDialog(QString warning_msg) {
	if (toolkit_settings.value("toolkit.showWarnings").isNull()) {
		//if the value is not set make default true 
		toolkit_settings.setValue("toolkit.showWarnings", true);
	}
	if (toolkit_settings.value("toolkit.showWarnings").toBool()) {
		//if warnings are enabled 
		QMessageBox warningBox;
		warningBox.setText(warning_msg);
		warningBox.setStandardButtons(QMessageBox::Ok);
		warningBox.setDefaultButton(QMessageBox::Ok);
		warningBox.exec();
	}
}

