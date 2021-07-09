#ifndef render_util_h_INCLUDED
#define render_util_h_INCLUDED

#include <Qt3DCore/QEntity>
#include <QColor>
#include "toolkit_config.h"

#include <rbdl/addons/luamodel/luatables.h>

TOOLKIT_DLLAPI Qt3DCore::QEntity* createGridFloor(float lxy_border,
                                                  float r_xyborder,
                                                  int count,
                                                  QColor line_color=QColor(QRgb(0xffffff)));

TOOLKIT_DLLAPI Qt3DCore::QEntity* createWire(const QVector3D& wire,
                                             const QColor& line_color,
                                             float line_width = 1.,
                                             Qt3DCore::QEntity* parent=nullptr);

TOOLKIT_DLLAPI Qt3DCore::QEntity* createMeshEntity(const QString& mesh_file,
                                                   const QColor& mesh_color,
                                                   const QVector3D& mesh_translation,
                                                   const QQuaternion& mesh_rotation,
                                                   Qt3DCore::QEntity* parent=nullptr);

#endif 

