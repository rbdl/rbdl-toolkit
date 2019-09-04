#include "AnimationModelExtention.h"

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

}
