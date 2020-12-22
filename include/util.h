#ifndef util_h_INCLUDED
#define util_h_INCLUDED

// a set of usefull standard functions, that may be needed by many components of the application

#include <QString>
#include <QStringList>
#include <QVector3D>
#include <string>

#include <rbdl/rbdl_math.h>

// returs an empty file on failure
QString findFile(QString file, bool absolute=false);
QString findFile(std::string file, bool absolute=false); 

QString findPlugin(QString plugin);
QStringList findAllPlugins();

QVector3D to_qt_vector(RigidBodyDynamics::Math::Vector3d&);

#endif 
