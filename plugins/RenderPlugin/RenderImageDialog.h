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

class RenderImageDialog : public QDialog, public Ui::RenderImageDialog {
    Q_OBJECT
 
public:
		RenderImageDialog (QWidget *parent = 0) {
			setupUi(this);
		}

};

#endif
