#ifndef ANIMATIONMODELEXTENTION_H_INCLUDED
#define ANIMATIONMODELEXTENTION_H_INCLUDED

#include <rbdl_wrapper.h>

#include <vector>

class AnimationModelExtension : public WrapperExtension {
	private:
		int dof;
		float max_time; 

		std::vector<float> animation_times;
		std::vector<RigidBodyDynamics::Math::VectorNd> animation_q_frames;

	public:
		AnimationModelExtension();

		std::string getExtensionName();
		void update(float current_time);

		int getDOF();
		float getMaxTime();

		void addAnimationFrame(float time, RigidBodyDynamics::Math::VectorNd& Q);

		//Data Mathods 
		const std::vector<float>& getAnimationTimes() { return animation_times; }
		const std::vector<RigidBodyDynamics::Math::VectorNd>& getAnimationFrames() { return animation_q_frames; }
};

#endif
