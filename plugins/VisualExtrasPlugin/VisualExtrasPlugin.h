#ifndef VISUALEXTRASPLUGIN_H_INCLUDED
#define VISUALEXTRASPLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>

#include "toolkit_interfaces.h"
#include "toolkit_errors.h"
#include "util.h"



class VisualExtrasPlugin : public QObject, public CoreInterface {
	Q_OBJECT
	Q_INTERFACES(CoreInterface)
	Q_PLUGIN_METADATA(IID CoreInterface_iid FILE "metadata.json")
	public:
		VisualExtrasPlugin();
		virtual ~VisualExtrasPlugin();

		void init(ToolkitApp* app);
		
	private:
		ToolkitApp* parentApp;
		
		void loadVisualExtrasSettings();
		
	public Q_SLOTS:
		
		void reload(RBDLModelWrapper* model);

};

#endif 
