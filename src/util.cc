#include "util.h"

#include "rbdl/rbdl_errors.h"
#include "toolkit_errors.h"

#include <sstream>
#include <iostream>

#include <QDir>
#include <QFileInfo>
#include <QLibrary>

QString findFile(QString file, bool absolute) {
	//if empty no file to be found
	if (file == "") {
		throw RigidBodyDynamics::Errors::RBDLInvalidFileError("Cannot find file with no name!");
	}

	QFileInfo check_file(file);
	if (check_file.exists()) {
		if (absolute)
			return check_file.absoluteFilePath();
		return file;
	}

	auto paths = QDir::searchPaths("");
	for (int i=0; i<paths.size(); i++) {
		QDir search_dir = QDir(paths.at(i));
		QString file_path = search_dir.filePath(file);
		auto info = QFileInfo(file_path);
		if (info.exists()) {
			if (absolute)
				return info.absoluteFilePath();
			return file_path;
		}
	}

	std::ostringstream errormsg;
	errormsg << "No file with filename: " << file.toStdString() << " found!" << std::endl;
	throw RigidBodyDynamics::Errors::RBDLInvalidFileError(errormsg.str());
}

QString findFile(std::string file, bool absolute) { 
	return findFile(QString::fromStdString(file), absolute); 
}

QString findPlugin(QString plugin) {
	QFileInfo check_file(plugin);
	if (check_file.exists()) {
		return plugin;
	}

	QString filename0 = QString("lib%1.so").arg(plugin);
	QFileInfo check_file0(filename0);
	if (check_file.exists()) {
		return filename0;
	}

	QString filename1 = QString("lib%1plugin.so").arg(plugin);
	QFileInfo check_file1(filename1);
	if (check_file.exists()) {
		return filename1;
	}

	auto paths = QDir::searchPaths("plugins");
	paths << "./plugins";
	for (auto path : paths) {
		QDir dir(path);
		if (dir.exists(plugin))
			return dir.filePath(plugin);
		if (dir.exists(filename0))
			return dir.filePath(filename0);
		if (dir.exists(filename1))
			return dir.filePath(filename1);
	}

	std::ostringstream errormsg;
	errormsg << "No plugin library matching the name " << plugin.toStdString() << " found!" << std::endl;
	throw RBDLFileNotFoundError(errormsg.str());
}

QStringList findAllPlugins() {
	QStringList plugin_list;

	auto paths = QDir::searchPaths("plugins");

	for (int i=0; i<paths.size(); i++) {
		QDir dir(paths.at(i));
		auto file_list = dir.entryList(QDir::Files);
		foreach (const QString &f, file_list) {
			auto f_info = QFileInfo(dir.path(), f);
			if (QLibrary::isLibrary(f_info.absoluteFilePath())) {
				plugin_list << f_info.absoluteFilePath();
			}
		}
	}

	return plugin_list;
}

QVector3D to_qt_vector(RigidBodyDynamics::Math::Vector3d& vec) {
	return QVector3D(vec[0], vec[1], vec[2]);
}
