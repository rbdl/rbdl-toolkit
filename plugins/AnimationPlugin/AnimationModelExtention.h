#ifndef ANIMATIONMODELEXTENTION_H_INCLUDED
#define ANIMATIONMODELEXTENTION_H_INCLUDED

#include <rbdl_wrapper.h>

#include <vector>

class AnimationModelExtention : public WrapperExtention {
	private:
		int dof;
		float max_time; 

		std::vector<float> animation_times;
		std::vector<RigidBodyDynamics::Math::VectorNd> animation_q_frames;

	public:
		AnimationModelExtention();

		std::string getExtentionName();
		void update(float current_time);

		int getDOF();
		float getMaxTime();

		void addAnimationFrame(float time, RigidBodyDynamics::Math::VectorNd& Q);
};

#endif
