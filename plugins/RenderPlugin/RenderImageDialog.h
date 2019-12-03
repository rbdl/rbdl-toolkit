/*
 * MeshUp - A visualization tool for multi-body systems based on skeletal
 * animation and magic.
 *
 * Copyright (c) 2012-2018 Martin Felis <martin.felis@iwr.uni-heidelberg.de>
 *
 * Licensed under the MIT license. See LICENSE for more details.
 */

#ifndef RENDERIMAGEDIALOG_H
#define RENDERIMAGEDIALOG_H
 
#include "ui_RenderImageDialog.h"
#include <QFileDialog>

class RenderImageDialog : public QDialog, public Ui::RenderImageDialog {
    Q_OBJECT
 
	public:
		RenderImageDialog (QWidget *parent = 0) {
			setupUi(this);

			connect(filenameSelect, &QPushButton::clicked, this, &RenderImageDialog::action_select_file);
		}

		void setFilename(QString text) {
			filenameEdit->setText(text);
		}

	public slots:

		void action_select_file() {
			QFileDialog file_dialog (this, "Select Image File");
			file_dialog.setFileMode(QFileDialog::AnyFile);

			if (file_dialog.exec()) {
				filenameEdit->setText(file_dialog.selectedFiles().at(0));
			}
		}

};

#endif
