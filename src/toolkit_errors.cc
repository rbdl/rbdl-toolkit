#include "toolkit_errors.h"

RBDLToolkitError::RBDLToolkitError(std::string text) : RigidBodyDynamics::Errors::RBDLError(text) {}
RBDLFileNotFoundError::RBDLFileNotFoundError(std::string text) : RBDLToolkitError(text) {}
