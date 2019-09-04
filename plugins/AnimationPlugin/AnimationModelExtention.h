#ifndef ANIMATIONMODELEXTENTION_H_INCLUDED
#define ANIMATIONMODELEXTENTION_H_INCLUDED

#include <rbdl_wrapper.h>

#include <vector>

class AnimationModelExtention : public WrapperExtention {
	private:
		int dof;
		float max_time; 

	public:
		AnimationModelExtention();

		std::string getExtentionName();
		void update(float current_time);

		int getDOF();
};

#endif

