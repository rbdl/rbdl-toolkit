#ifndef SCENEGROUPSWIDGET_H_INCLUDED
#define SCENEGROUPSWIDGET_H_INCLUDED

#include <QWidget>
#include <QCheckBox>
#include "ui_SceneGroupList.h"

class SceneGroupsWidget : public QWidget, public Ui::SceneGroupList {
	Q_OBJECT
	private:
		int label_pos;
		int size;
	public:
		SceneGroupsWidget(QWidget* parent): QWidget(parent) {
			setupUi(this);

			groupLayout->setDirection(QBoxLayout::TopToBottom);
			groupLayout->setAlignment(Qt::AlignTop);
			groupLayout->setSpacing(0);
			label_pos = 0;
			size = 1;
		}

		void addSelector(QCheckBox* widget) {
			bool ok = true;
			QString check = widget->text().split(":")[0];
			check.toInt(&ok);

			if (ok) {
				//is model toggle
				groupLayout->addWidget(widget, 0, Qt::AlignTop);
				size++;
			} else {
				for (int pos=0; pos<size; pos++) {
					if (pos == label_pos) {
						groupLayout->insertWidget(pos, widget, 0, Qt::AlignTop);
						break;
					}
					QCheckBox* box = qobject_cast<QCheckBox*>(groupLayout->itemAt(pos)->widget());
					if (box != nullptr) {
						if (widget->text() < box->text()) {
							groupLayout->insertWidget(pos, widget, 0, Qt::AlignTop);
							break;
						}
					}
				}

				label_pos++;
				size++;
			}

		}
};

#endif 

