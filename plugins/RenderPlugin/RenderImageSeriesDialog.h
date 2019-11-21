/*
 * MeshUp - A visualization tool for multi-body systems based on skeletal
 * animation and magic.
 *
 * Copyright (c) 2012-2018 Martin Felis <martin.felis@iwr.uni-heidelberg.de>
 *
 * Licensed under the MIT license. See LICENSE for more details.
 */

#ifndef RENDERIMAGESERIESDIALOG_H
#define RENDERIMAGESERIESDIALOG_H
 
 

#include "ui_RenderImageSeriesDialog.h"

class RenderImageSeriesDialog : public QDialog, public Ui::RenderImageSeriesDialog {
    Q_OBJECT
 
public:
		RenderImageSeriesDialog (QWidget *parent = 0) {
			setupUi(this);
		}

};
#endif
