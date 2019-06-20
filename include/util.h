#ifndef util_h_INCLUDED
#define util_h_INCLUDED

// a set of usefull standard funktion that are needed by many components of the application

#include <QString>
#include <string>

// returs an empty file on failure
QString findFile(QString file);

QString findFile(std::string file); 

#endif 
