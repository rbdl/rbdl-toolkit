#ifndef VECTOREDITOR_H_INCLUDED
#define VECTOREDITOR_H_INCLUDED

#include <QDialog>
#include <QVector3D>
#include "toolkit_config.h"

#include "ui_VectorEditor.h"

class TOOLKIT_DLLAPI VectorEditDialog : public QDialog, public Ui::VectorEditor {
	Q_OBJECT
	public:
		VectorEditDialog(QWidget* parent,  QVector3D& val) {
			setupUi(this);

			xValueBox->setValue(val.x());
			yValueBox->setValue(val.y());
			zValueBox->setValue(val.z());
		}

		const QVector3D getValue() {
			return QVector3D(xValueBox->value(), yValueBox->value(), zValueBox->value());
		}
};

#endif

