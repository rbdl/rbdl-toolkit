#ifndef util_h_INCLUDED
#define util_h_INCLUDED

// a set of usefull standard functions, that may be needed by many components of the application

#include <QString>
#include <QStringList>
#include <QVector3D>
#include <string>

#include <rbdl/rbdl_math.h>
#include "toolkit_config.h"

// returs an empty file on failure
TOOLKIT_DLLAPI QString findFile(QString file, bool absolute=false);
TOOLKIT_DLLAPI QString findFile(std::string file, bool absolute=false);

TOOLKIT_DLLAPI QString findPlugin(QString plugin);
TOOLKIT_DLLAPI QStringList findAllPlugins();

TOOLKIT_DLLAPI QVector3D to_qt_vector(RigidBodyDynamics::Math::Vector3d&);

#endif 
