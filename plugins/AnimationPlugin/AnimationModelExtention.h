#ifndef ANIMATIONMODELEXTENTION_H_INCLUDED
#define ANIMATIONMODELEXTENTION_H_INCLUDED

#include <rbdl_wrapper.h>

class AnimationModelExtention : public WrapperExtention {
	public:
		std::string getExtentionName();
		void update(float current_time);
};

#endif

