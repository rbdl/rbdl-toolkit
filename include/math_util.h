#ifndef MATH_UTIL_H_INCLUDED
#define MATH_UTIL_H_INCLUDED

#include "rbdl/rbdl_math.h"
#include "toolkit_config.h"

using namespace RigidBodyDynamics::Math;

TOOLKIT_DLLAPI VectorNd interpolate_values_linear(const VectorNd& data0, const VectorNd& data1, float time_fraction);

#endif 
