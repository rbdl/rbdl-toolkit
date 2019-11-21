// RenderVideoDialog.h
// header file for render video dialog box
// Author: Stephen Meisenbacher

#ifndef RENDERVIDEODIALOG_H
#define RENDERVIDEODIALOG_H

#include <QValidator>
#include <QSpinBox>
#include <QAbstractButton>

class RenderVideoDialog;

#include "ui_RenderVideoDialog.h"


class RenderVideoDialog : public QDialog, public Ui::RenderVideoDialog {
    Q_OBJECT

	public:
	    RenderVideoDialog (QWidget *parent = 0) {
			setupUi(this);
			connect(HeightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(checkValues()));
			connect(WidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(checkValues()));
			checkValues();
		}
		void set_video_lenght(double len) {
			TimeSpinBox->setValue(len);
		}

	public slots:
		void checkValues() {
			QAbstractButton *accept_but;
			for (int i=0; i < buttonBox->buttons().size(); i++) {
				if ( buttonBox->buttonRole(buttonBox->buttons().at(i)) == QDialogButtonBox::AcceptRole ) {
					accept_but = buttonBox->buttons().at(i);
					break;
				}
			}
			if ( HeightSpinBox->value() % 8 != 0 || WidthSpinBox->value() % 8 != 0 ) {
				accept_but->setEnabled(false);
				warningLabel->setText("The width and height of the video need to be devisable by 8!");
			} else {
				accept_but->setEnabled(true);
				warningLabel->setText("");
			}
		}

};
#endif
