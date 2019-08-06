#ifndef TOOLKIT_INTERFACES_H_INCLUDED
#define TOOLKIT_INTERFACES_H_INCLUDED

class ViewInterface {
	public:
		virtual ~ViewInterface();

		virtual QString getViewName() = 0;
		virtual QDockWidget* getViewWidget() = 0;
};

class ModelInterface {
	public:
		virtual ~ModelInterface();
};

class FileInterface {
	public:
		virtual ~FileInterface();
};


#endif 

