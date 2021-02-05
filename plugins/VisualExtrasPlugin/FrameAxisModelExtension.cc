#include "FrameAxisModelExtension.h"



FrameAxisModelExtension::FrameAxisModelExtension() : WrapperExtension() 
{
}

std::string FrameAxisModelExtension::getExtensionName() {
	return "FrameAxis";
}

void FrameAxisModelExtension::update(float current_time) {
	//nothing to update in sync with time
}
