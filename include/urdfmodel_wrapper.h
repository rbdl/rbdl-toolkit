#ifndef URDFMODEL_WRAPPER_H_INCLUDED
#define URDFMODEL_WRAPPER_H_INCLUDED

#include <rbdl/rbdl.h>
#include <rbdl/rbdl_math.h>
#include <rbdl/addons/urdfreader/urdfreader.h>

#include "rbdl_wrapper.h"

#define MODELTYPE_URDF "urdf_model"

class TOOLKIT_DLLAPI UrdfModelWrapper : public RBDLModelWrapper {
	public:
		UrdfModelWrapper();

		std::string model_xml_string;

		void load(QString model_file);
		ModelInfo loadModelInfo();
		std::vector<SegmentVisualInfo> loadSegmentInfo();
};


#endif 

