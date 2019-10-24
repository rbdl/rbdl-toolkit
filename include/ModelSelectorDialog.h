#ifndef ModelSelectorDialog_h_INCLUDED
#define ModelSelectorDialog_h_INCLUDED

#include <QDialog>
#include <QListWidgetItem>
#include <QPushButton>
#include "ui_ModelSelector.h"

#include <rbdl_wrapper.h>

typedef bool (*ModelFilter)(RBDLModelWrapper* model);
typedef std::vector<RBDLModelWrapper*>* ModelListRef;

class ModelListItem : public QListWidgetItem {
	public:
		ModelListItem(RBDLModelWrapper* model, QListWidget* parent=0);

		RBDLModelWrapper* model_ref;
};

class ModelSelectorDialog : public QDialog, public Ui::ModelSelector {
	Q_OBJECT
	private:
		bool filter_models;
		ModelFilter filter;

		ModelListRef models;

		QPushButton* ok;
	public:
		ModelSelectorDialog( ModelListRef model_list, 
		                     ModelFilter filter_func=nullptr, 
		                     QWidget *parent=nullptr);

		RBDLModelWrapper* getSelectetModel();
		std::vector<ModelListItem*> list_items;

	public slots:
		void update_model_list(int filter_enabled);
		void model_selected();
};

#endif 

