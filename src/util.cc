#include "util.h"

#include "rbdl/rbdl_errors.h"
#include <sstream>

#include <QDir>
#include <QFileInfo>

QString findFile(QString file) {
	//if empty no file to be found
	if (file == "") {
		throw RigidBodyDynamics::Errors::RBDLInvalidFileError("Cannot find file with no name!");
	}

	QFileInfo check_file(file);
	if (check_file.exists()) {
		return file;
	}

	auto paths = QDir::searchPaths("");
	for (int i=0; i<paths.size(); i++) {
		QDir search_dir = QDir(paths.at(i));
		QString file_path = search_dir.filePath(file);
		if (QFileInfo(file_path).exists()) {
			return file_path;
		}
	}
	std::ostringstream errormsg;
	errormsg << "No file with filename: " << file.toStdString() << " found!" << std::endl;
	throw RigidBodyDynamics::Errors::RBDLInvalidFileError(errormsg.str());

}

QString findFile(std::string file) { 
	return findFile(QString::fromStdString(file)); 
}
