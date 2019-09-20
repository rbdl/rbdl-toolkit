#include "math_util.h"

#include "rbdl/rbdl_errors.h"

VectorNd interpolate_values_linear(const VectorNd& data0, const VectorNd& data1, float time_fraction) {
	if ( data0.size() != data1.size() ) {
		throw RigidBodyDynamics::Errors::RBDLSizeMismatchError("The vectors you wish to interpolate are of different sizes!");
	}

	if ( time_fraction < 0. || time_fraction > 1. ) {
		throw RigidBodyDynamics::Errors::RBDLInvalidParameterError("The time fraction must be in range 0. to 1.!");
	}

	VectorNd step = (data0 - data1);

	return data0 + ( time_fraction * step );
}
