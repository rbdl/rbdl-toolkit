#include "ModelSelectorDialog.h"

#include <QFileInfo>
#include <QDialogButtonBox>

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
		update_model_list(2);
	} else {
		filter_models = true;
		update_model_list(0);
	}

	ok = buttonBox->button(QDialogButtonBox::Ok);
	ok->setDisabled(true);

	connect(checkBoxShowAll, SIGNAL(stateChanged(int)), this, SLOT(update_model_list(int)));
	connect(listWidgetModelList, SIGNAL(itemSelectionChanged(void)), this, SLOT(model_selected(void)));
}

RBDLModelWrapper* ModelSelectorDialog::getSelectetModel() {
	auto selected = listWidgetModelList->selectedItems()[0];
	return ((ModelListItem*)selected)->model_ref;
}

void ModelSelectorDialog::model_selected() {
	if (listWidgetModelList->selectedItems().size() > 0) {
		ok->setEnabled(true);
	} else {
		ok->setEnabled(false);
	}
}

void ModelSelectorDialog::update_model_list(int filter_enabled) {
	if (filter_enabled == 2) {
		filter_models = false;
	} else {
		filter_models = true;
	}

	if (list_items.size() > 0) {
		for (auto item : list_items) {
			delete item;
		}
		list_items.clear();
	}

	for (RBDLModelWrapper* model : *models) {
		if (filter_models) {
			if (filter(model)) {
				ModelListItem* model_item = new ModelListItem(model, listWidgetModelList);
				list_items.push_back(model_item);
				listWidgetModelList->addItem(model_item);
			}
		} else {
			ModelListItem* model_item = new ModelListItem(model, listWidgetModelList);
			list_items.push_back(model_item);
			listWidgetModelList->addItem(model_item);
		}
	}
}

ModelListItem::ModelListItem(RBDLModelWrapper* model, QListWidget* parent): model_ref(model) {
	QString model_file = QFileInfo(model->getModelFile()).baseName();
	setText(QString("%1\t(dof=%2)").arg(model_file, QString::number(model->getModelDof())));
}

