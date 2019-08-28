#ifndef TOOLKIT_ERRORS_H_INCLUDED
#define TOOLKIT_ERRORS_H_INCLUDED

#include <rbdl/rbdl_errors.h>

class RBDLToolkitError : public RigidBodyDynamics::Errors::RBDLError {
	public:
		RBDLToolkitError(std::string text);
};

#endif
