#include "AnimationPlugin.h"
#include <iostream>

AnimationPlugin::AnimationPlugin() {
	parentApp = NULL;
}

AnimationPlugin::~AnimationPlugin() {
}

void AnimationPlugin::init(ToolkitApp* app) {
	//save reference to parent ToolkitApp 
	parentApp = app;

	std::cout << "Core Plugin for Animations loaded!" << std::endl;
}

