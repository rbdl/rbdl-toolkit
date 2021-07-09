#include "toolkit_errors.h"

TOOLKIT_DLLAPI RBDLToolkitError::RBDLToolkitError(std::string text) : RigidBodyDynamics::Errors::RBDLError(text) {}
TOOLKIT_DLLAPI RBDLFileNotFoundError::RBDLFileNotFoundError(std::string text) : RBDLToolkitError(text) {}
