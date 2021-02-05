#ifndef FRAMEAXISMODELEXTENSION_H_INCLUDED
#define FRAMEAXISMODELEXTENSION_H_INCLUDED

#include <rbdl_wrapper.h>


class FrameAxisModelExtension : public WrapperExtension {
	private:
		// Vars
		// 
		// int dof;
		// float max_time;
	public:
		FrameAxisModelExtension();

		// These Functions need to be implemented
		std::string getExtensionName();
		void update(float current_time);

		// Optional Model wrapper virtual functions
		// Qt3DCore::QEntity* getVisual();
		// void exportData();

		// Getter and Setter functions
		// int getDOF();
		// float getMaxTime();

};

#endif