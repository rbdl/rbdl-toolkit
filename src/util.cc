#include "util.h"

#include <QDir>
#include <QFileInfo>

QString findFile(QString file) {
	//if empty just return no file to be found
	if (file == "") {
		return file;
	}

	QFileInfo check_file(file);
	if (check_file.exists()) {
		return file;
	}

	auto paths = QDir::searchPaths("");
	for (int i=0; i<paths.size(); i++) {
		QDir search_dir = QDir(paths.at(i));
		QString file_path = search_dir.filePath(file);
		if (search_dir.exists()) {
			return file_path;
		}
	}
	return QString("");

}

QString findFile(std::string file) { 
	return findFile(QString::fromStdString(file)); 
}
