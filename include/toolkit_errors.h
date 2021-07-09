#ifndef TOOLKIT_ERRORS_H_INCLUDED
#define TOOLKIT_ERRORS_H_INCLUDED

#include <rbdl/rbdl_errors.h>
#include <toolkit_config.h>

class TOOLKIT_DLLAPI RBDLToolkitError : public RigidBodyDynamics::Errors::RBDLError {
	public:
		RBDLToolkitError(std::string text);
};

class TOOLKIT_DLLAPI RBDLFileNotFoundError: public RBDLToolkitError {
	public:
		RBDLFileNotFoundError(std::string text);
};

#endif
