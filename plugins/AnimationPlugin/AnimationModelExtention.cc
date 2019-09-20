#include "AnimationModelExtention.h"

#include "rbdl/rbdl_errors.h"

AnimationModelExtention::AnimationModelExtention() : dof(0),
                                                     max_time(0.),
                                                     WrapperExtention() 
{
}

std::string AnimationModelExtention::getExtentionName() {
	return "Animation";
}

void AnimationModelExtention::update(float current_time) {
	//TODO
}

int AnimationModelExtention::getDOF() {
	return dof;
}

void AnimationModelExtention::addAnimationFrame(float time, RigidBodyDynamics::Math::VectorNd& Q) {
	//if first entry then update dof
	if (dof == 0 && Q.size() != 0) {
		dof = Q.size();

	//check if dof of data matches that of the other entries
	} else if ( dof != Q.size() ) {
		throw RigidBodyDynamics::Errors::RBDLDofMismatchError("The size of the state vector does not match previous state vectors!");
	}

	//check time values to be sequential
	if (time < animation_times[animation_times.size()-1] ) {
		throw RigidBodyDynamics::Errors::RBDLInvalidParameterError("Time values must be sequential!");
	}

	animation_times.push_back(time);
	max_time = time;

	animation_q_frames.push_back(Q);
}
