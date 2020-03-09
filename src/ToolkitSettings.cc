#include "ToolkitSettings.h"

#include <QTreeWidget>
#include <iostream>

ToolkitSettings::ToolkitSettings() {
}

QVariant ToolkitSettings::value(const QString &key) {
	return settings.value(key);
}

QMetaType::Type ToolkitSettings::type(const QString &key) {
	if (settings.group() == "") 
		return value_type_map[key];
	return value_type_map[QString("%1:%2").arg(settings.group(), key)];
}

void ToolkitSettings::setValue(const QString &key, const QVariant &value) {
	setType(key, value);
	settings.setValue(key, value);
}

void ToolkitSettings::setType(const QString &key, const QVariant infer_from_value) {
	if (settings.group() == "") {
		value_type_map[key] = (QMetaType::Type)(infer_from_value.type());
	} else {
		value_type_map[QString("%1:%2").arg(settings.group(), key)] = (QMetaType::Type)(infer_from_value.type());
	}
}

void ToolkitSettings::beginGroup(QString group) {
	settings.beginGroup(group);
}

void ToolkitSettings::endGroup() {
	settings.endGroup();
}

void ToolkitSettings::editSettings() {
	SettingsEditor* edit = new SettingsEditor(this);
	edit->show();
}

SettingsEditor::SettingsEditor(ToolkitSettings* settings) : settings(settings), QWidget() {
	setupUi(this);

	settings_tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

	group_icon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon), QIcon::Normal, QIcon::Off);
	group_icon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon), QIcon::Normal, QIcon::On);
	key_icon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

	updateChildItems(nullptr);
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
	item->setFlags(item->flags() | Qt::ItemIsEditable);
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
		child->setText(1, QMetaType::typeName(settings->type(key)));
		child->setText(2, settings->value(key).toString());
	}
}
