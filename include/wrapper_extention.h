#ifndef WRAPPER_EXTENTION_H_INCLUDED
#define WRAPPER_EXTENTION_H_INCLUDED

/* This class provides an abstact base for adding data to a loaded model, such as (Animations, Forces, etc.).
 */

class WrapperExtention {
	protected:
		std::string extention_name;

	public:
		WrapperExtention();

		std::string getExtentionName() { return extention_name; }

};

#endif 

