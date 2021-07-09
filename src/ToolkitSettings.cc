#include "ToolkitSettings.h"

#include <QTreeWidget>
#include <QColorDialog>
#include <QInputDialog>
#include <iostream>

#include <variantdelegate.h>
#include "VectorEditor.h"

TOOLKIT_DLLAPI ToolkitSettings::ToolkitSettings() {
}

TOOLKIT_DLLAPI QVariant ToolkitSettings::value(const QString &key) {
	return settings.value(key);
}

TOOLKIT_DLLAPI QMetaType::Type ToolkitSettings::type(const QString &key) {
	if (settings.group() == "") 
		return value_type_map[key];
	return value_type_map[QString("%1:%2").arg(settings.group(), key)];
}

TOOLKIT_DLLAPI void ToolkitSettings::setValue(const QString &key, const QVariant &value) {
	setType(key, value);
	settings.setValue(key, value);
}

TOOLKIT_DLLAPI void ToolkitSettings::setType(const QString &key, const QVariant infer_from_value) {
	if (settings.group() == "") {
		value_type_map[key] = (QMetaType::Type)(infer_from_value.type());
	} else {
		value_type_map[QString("%1:%2").arg(settings.group(), key)] = (QMetaType::Type)(infer_from_value.type());
	}
}

TOOLKIT_DLLAPI void ToolkitSettings::beginGroup(QString group) {
	settings.beginGroup(group);
}

TOOLKIT_DLLAPI void ToolkitSettings::endGroup() {
	settings.endGroup();
}

TOOLKIT_DLLAPI void ToolkitSettings::editSettings() {
	SettingsEditor *edit = new SettingsEditor(this);
	edit->setWindowFlags(Qt::Dialog);
	connect(edit, &SettingsEditor::settings_changed, this, [=]() { emit settings_changed(); });
	edit->show();
}

SettingsEditor::SettingsEditor(ToolkitSettings* settings) : settings(settings), QWidget() {
	setupUi(this);

	settings_tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	settings_tree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

	group_icon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon), QIcon::Normal, QIcon::Off);
	group_icon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon), QIcon::Normal, QIcon::On);
	key_icon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

	updateChildItems(nullptr);

	connect(settings_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(editSetting(QTreeWidgetItem*, int)));
}

void SettingsEditor::resizeEvent(QResizeEvent* event) {
	settings_tree->setGeometry(QRect(0, 0, event->size().width(), event->size().height()));
}

QTreeWidgetItem* SettingsEditor::createItem(const QString &text, QTreeWidgetItem* parent, int index) {
	QTreeWidgetItem* after = nullptr;
	if (index != 0)
		after = childAt(parent, index-1);

	QTreeWidgetItem* item;
	if (parent)
		item = new QTreeWidgetItem(parent, after);
	else
		item = new QTreeWidgetItem(settings_tree, after);

	item->setText(0, text);
	item->setFlags(item->flags());
	return item;
}

QTreeWidgetItem* SettingsEditor::childAt(QTreeWidgetItem* parent, int index) const {
	return (parent ? parent->child(index) : settings_tree->topLevelItem(index)); 
}

int SettingsEditor::childCount(QTreeWidgetItem* parent) const {
	return (parent ? parent->childCount() : settings_tree->topLevelItemCount() ); 
}

int SettingsEditor::findChild(QTreeWidgetItem* parent, const QString &text, int starti) const {
	for (int i=starti; i<childCount(parent); i++) {
		if(childAt(parent,i)->text(0) == text)
			return i;
	}
	return -1;
}

void SettingsEditor::updateChildItems(QTreeWidgetItem *parent) {
	int div_i = 0;

	//create sub group nodes
	const QStringList child_groups = settings->childGroups();
	for (const QString &group : child_groups) {
		QTreeWidgetItem *child;
		int child_index = findChild(parent, group, div_i);
		if(child_index != -1) {
			child = childAt(parent, child_index);
			child->setText(1, QString());
			child->setText(2, QString());
			child->setData(2, Qt::UserRole, QVariant());
		} else {
			child = createItem(group, parent, div_i);
		}
		child->setIcon(0, group_icon);
		div_i++;

		settings->beginGroup(group);
		updateChildItems(child);
		settings->endGroup();
	}

	//create settings entries
	const QStringList childKeys = settings->childKeys();
	for (const QString &key : childKeys) {
		QTreeWidgetItem *child;
		int child_index = findChild(parent, key, 0);

		if (child_index == -1) {
			child = createItem(key, parent, div_i);
			child->setIcon(0, key_icon);
			div_i++;
		} else {
			child = childAt(parent, child_index);
		}
		child->setData(0, Qt::UserRole, settings->group());
		child->setText(1, QMetaType::typeName(settings->type(key)));
		child->setData(1, Qt::UserRole, QMetaType::typeName(settings->type(key)));
		child->setText(2, VariantDelegate::displayText(settings->value(key), settings->type(key)));
		child->setData(2, Qt::UserRole, settings->value(key));
		if (settings->type(key) == QMetaType::QColor) {
			QPixmap color_icon(10,10);
			color_icon.fill(QColor::fromRgba(settings->value(key).toUInt()));
			child->setIcon(2, color_icon);
		}
	}
}

void SettingsEditor::editSetting(QTreeWidgetItem* item, int column) {
	QMetaType::Type type = (QMetaType::Type)QMetaType::type(item->data(1, Qt::UserRole).toString().toStdString().c_str());
	QString group = item->data(0, Qt::UserRole).toString();

	bool changed = false;

	switch(type) {
		case QMetaType::QColor: {
			QColor current_color = QColor::fromRgba(item->data(2, Qt::UserRole).toUInt());
			QColorDialog color_picker(current_color, this);
			color_picker.setOption(QColorDialog::ShowAlphaChannel);
			if (color_picker.exec()) {
				changed = true;
				QColor selected = color_picker.selectedColor();
				if (group != "") 
					settings->beginGroup(group);
				settings->setValue(item->text(0), selected.rgba());
				settings->setType(item->text(0), selected);
				if (group != "") 
					settings->endGroup();
			}
			break;
		}
		case QMetaType::Bool: {
			bool current_state = item->data(2, Qt::UserRole).toBool();
			QStringList states;
			states << "false" << "true";
			bool ok;
			QString selection = QInputDialog::getItem(this, 
			                                          tr("Select State"), 
			                                          "Bool:", 
			                                          states,
			                                          current_state ? 1 : 0,
			                                          false, &ok);
			if (ok) {
				bool state = (selection == "true") ? true : false;
				if (state != current_state) {
					changed = true;
					if (group != "") 
						settings->beginGroup(group);
					settings->setValue(item->text(0), state);
					if (group != "") 
						settings->endGroup();
				}
			}
			break;
		}
		case QMetaType::QChar: {
		    QString current_char(QChar::fromLatin1((char) item->data(2, Qt::UserRole).toUInt()));
			bool ok;
			QString new_char = QInputDialog::getText(this, 
			                                         tr("Choose Character"), 
			                                         "Type char:", 
			                                         QLineEdit::Normal,
			                                         current_char,
			                                         &ok);
			if (ok && new_char.size() == 1) {
				changed = true;
				char s = new_char.toStdString().c_str()[0];
				if (group != "") 
					settings->beginGroup(group);
				settings->setValue(item->text(0), s);
				settings->setType(item->text(0), (QChar)s);
				if (group != "") 
					settings->endGroup();
			}
			break;
		}
		case QMetaType::Double:
		case QMetaType::Float: {
			double current_val = item->data(2, Qt::UserRole).toDouble();
			bool ok;
			double new_val = QInputDialog::getDouble(this,
			                                         tr("Choose Value"),
			                                         tr("Value"),
			                                         current_val,
			                                         -2147483647,
			                                         2147483647,
			                                         9,
			                                         &ok);
			if (ok) {
				changed = true;
				if (group != "") 
					settings->beginGroup(group);
				settings->setValue(item->text(0), new_val);
				if (group != "") 
					settings->endGroup();
			}
			break;
		}
		case QMetaType::UInt: {
			double current_val = item->data(2, Qt::UserRole).toUInt();
			bool ok;
			double new_val = QInputDialog::getInt(this,
			                                         tr("Choose Value"),
			                                         tr("Value"),
			                                         current_val,
			                                         -2147483647,
			                                         2147483647,
			                                         1,
			                                         &ok);
			if (ok) {
				changed = true;
				if (group != "") 
					settings->beginGroup(group);
				settings->setValue(item->text(0), new_val);
				if (group != "") 
					settings->endGroup();
			}
			break;
		}
		case QMetaType::QVector3D: {
			QVector3D current_val = item->data(2, Qt::UserRole).value<QVector3D>();

			VectorEditDialog vec_edit(this, current_val);
			int result = vec_edit.exec();
			if (result == QDialog::Rejected)
				break;

			changed = true;
			QVector3D new_val = vec_edit.getValue();
			if (group != "") 
				settings->beginGroup(group);
			settings->setValue(item->text(0), new_val);
			if (group != "") 
				settings->endGroup();
			break;
		}
		default:
			break;
	}

	if (changed) {
		updateChildItems(nullptr);
		emit settings_changed();
	}
}
