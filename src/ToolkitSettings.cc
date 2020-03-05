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

	for (auto key : settings->childKeys()) {
		QStringList display;
		display << key;
		QString type_str = QMetaType::typeName(settings->type(key));
		display << type_str;
		QString value = settings->value(key).toString();
		display << value;
		settings_items.append(new QTreeWidgetItem((QTreeWidget*)nullptr, display));
	}

	settings_tree->insertTopLevelItems(0, settings_items);
}

void SettingsEditor::resizeEvent(QResizeEvent* event) {
	settings_tree->setGeometry(QRect(0, 0, event->size().width(), event->size().height()));
}
