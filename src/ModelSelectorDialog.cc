#include "ModelSelectorDialog.h"

#include <QFileInfo>

ModelSelectorDialog::ModelSelectorDialog( 
	ModelListRef model_list,
	ModelFilter filter_func,
	QWidget *parent 
) : models(model_list), filter(filter_func), QDialog(parent)
{
	setupUi(this);

	if ( filter == nullptr ) {
		filter_models = false;
		checkBoxShowAll->setCheckState(Qt::Checked);
		checkBoxShowAll->setDisabled(true);
	} else {
		filter_models = true;
	}

	for (RBDLModelWrapper* model : *model_list) {
		ModelListItem* model_item = new ModelListItem(model, listWidgetModelList);
		list_items.push_back(model_item);
		listWidgetModelList->addItem(model_item);
	}
}

RBDLModelWrapper* ModelSelectorDialog::getSelectetModel() {
	auto selected = listWidgetModelList->selectedItems()[0];
	return ((ModelListItem*)selected)->model_ref;
}

ModelListItem::ModelListItem(RBDLModelWrapper* model, QListWidget* parent): model_ref(model) {
	QString model_file = model->getModelFile();
	setText(QFileInfo(model_file).baseName());
}
