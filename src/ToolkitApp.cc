#include "ToolkitApp.h"
#include "render_util.h"
#include "util.h"
#include "rbdl/rbdl_errors.h"

#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>
#include <QKeySequence>
#include <QMessageBox>
#include <Qt3DCore>
#include <QCoreApplication>

ToolkitApp::ToolkitApp(QWidget *parent) {
	main_menu_bar = new QMenuBar(NULL);
	file_menu = main_menu_bar->addMenu("File");
	view_menu = main_menu_bar->addMenu("Views");
	plugin_menu = main_menu_bar->addMenu("Plugins");

	file_menu->addAction("Load Model", this, "aaction_load_model()");
	file_menu->addAction("Reload Files", this, "aaction_reload_files()", QKeySequence::fromString("F5"));

	this->setMenuBar(main_menu_bar);

	main_display = new SceneWidget(this);
	this->setCentralWidget(main_display);

	main_display->addSceneObject(createGridFloor(-15., 15., 32));

	//set standard search paths
	QDir::addSearchPath("", "meshes/");
	auto paths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
	for (int i=0; i<paths.size(); i++) {
		QFileInfo check_file(paths.at(i));
		if (check_file.exists()) {
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
		loaded_models.push_back(model);
		main_display->addSceneObject(model_scene_obj);
	}

}

void ToolkitApp::addView(QString name, QWidget *view_widget, Qt::DockWidgetArea area) {
	QDockWidget* dock = new QDockWidget(name, this);
	dock->setWidget(view_widget);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	addDockWidget(area, dock);

	view_widgets.push_back(dock);
	view_menu->addAction(dock->toggleViewAction());
}

void ToolkitApp::initPlugins() {
	//create list of availible plugins to load and put them in menu
	auto plugins = findAllPlugins();
	//std::cout << "Found " << plugins.size() << " plugins to be loaded!" << std::endl;

	int i = 0;
	foreach (const QString plugin_path, plugins) {
		availible_plugins.push_back(new QPluginLoader(plugin_path));

		//create menu action to enable and disable plugins
		QString action_name = availible_plugins[i]->metaData().value("MetaData").toObject().value("Name").toString();
		QAction *plugin_action = plugin_menu->addAction(action_name); 
		plugin_action->setCheckable(true);

		//connect to action via lambda function
		connect(plugin_action, &QAction::changed, [=] 
		{ 
			setPluginUsage(i, plugin_action->isChecked()); 
		});

		i += 1;
	}
}

void ToolkitApp::setPluginUsage(unsigned int plugin_ref, bool state) {
	std::cout << "Setting mode of plugin " << plugin_ref << " to " << state << std::endl; 

	//Todo actually load plugin
}

