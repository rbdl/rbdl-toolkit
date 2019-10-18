#include "AnimationModelExtention.h"

#include "rbdl/rbdl_errors.h"

#include "math_util.h"

AnimationModelExtention::AnimationModelExtention() : dof(0),
                                                     max_time(0.),
                                                     WrapperExtention() 
{
}

std::string AnimationModelExtention::getExtentionName() {
	return "Animation";
}

void AnimationModelExtention::update(float current_time) {
	unsigned int time_index = 0;
	for (; time_index < animation_times.size(); time_index++) {
		if (time_index == animation_times.size()) break;
		if (animation_times[time_index] <= current_time && animation_times[time_index+1] > current_time) {
			break;
		}
	}
	if (time_index == animation_times.size()) {
		model_parent->updateKinematics(animation_q_frames[time_index-1]);
		return;
	}

	float start_time = animation_times[time_index];
	float end_time = animation_times[time_index+1];

	float time_fraction = (current_time - start_time) / (end_time - start_time);

	RigidBodyDynamics::Math::VectorNd interpolated_q = interpolate_values_linear(
		animation_q_frames[time_index], 
		animation_q_frames[time_index+1],
		time_fraction
	);

	model_parent->updateKinematics(interpolated_q);
}

int AnimationModelExtention::getDOF() {
	return dof;
}

float AnimationModelExtention::getMaxTime() {
	return max_time;
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
	if ( animation_times.size() != 0 && time < animation_times[animation_times.size()-1] ) {
		throw RigidBodyDynamics::Errors::RBDLInvalidParameterError("Time values must be sequential!");
	}

	animation_times.push_back(time);
	max_time = time;

	animation_q_frames.push_back(Q);
}
