#ifndef MATH_UTIL_H_INCLUDED
#define MATH_UTIL_H_INCLUDED

#include "rbdl/rbdl_math.h"

using namespace RigidBodyDynamics::Math;

VectorNd interpolate_values_linear(const VectorNd& data0, const VectorNd& data1, float time_fraction);

#endif 
