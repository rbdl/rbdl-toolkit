#ifndef util_h_INCLUDED
#define util_h_INCLUDED

// a set of usefull standard functions, that may be needed by many components of the application

#include <QString>
#include <QStringList>
#include <string>

// returs an empty file on failure
QString findFile(QString file);
QString findFile(std::string file); 

QString findPlugin(QString plugin);
QStringList findAllPlugins();

#endif 
