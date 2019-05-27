#ifndef render_util_h_INCLUDED
#define render_util_h_INCLUDED

#include <Qt3DCore/QEntity>
#include <QColor>

#include <rbdl/addons/luamodel/luatables.h>

Qt3DCore::QEntity* createGridFloor(float lxy_border, float r_xyborder, int count, QColor line_color=QColor(QRgb(0xffffff)));
//Qt3DCore::QEntity* loadFromLuaTable(LuaTableNode &table);

#endif 

