#ifndef ToolkitSettings_h_INCLUDED
#define ToolkitSettings_h_INCLUDED

#include <QSettings>
#include <QString>
#include <QMetaType>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QResizeEvent>
#include <QIcon>

#include "ui_SettingsEditor.h"

class ToolkitSettings;

class SettingsEditor : public QWidget, public Ui::SettingsEditor {
	Q_OBJECT
	private:
		ToolkitSettings* settings;

		void updateChildItems(QTreeWidgetItem *parent);
		QTreeWidgetItem* createItem(const QString &text, QTreeWidgetItem* parent, int index);
		QTreeWidgetItem* childAt(QTreeWidgetItem* parent, int index) const;
		int findChild(QTreeWidgetItem* parent, const QString &text, int starti) const;
		int childCount(QTreeWidgetItem* parent) const;

		QIcon group_icon, key_icon;

	public:
		SettingsEditor(ToolkitSettings* settings);

		void resizeEvent(QResizeEvent *event);
};


class ToolkitSettings : public QObject{
	Q_OBJECT
	public:
		ToolkitSettings();

		QVariant value(const QString &key);
		QMetaType::Type type(const QString &key);
		void setValue(const QString &key, const QVariant &value);
		void setType(const QString &key, const QVariant infer_from_value);

		void beginGroup(QString group);
		void endGroup();

		QStringList childKeys() { return settings.childKeys(); }
		QStringList childGroups() { return settings.childGroups(); }

	private:
		QSettings settings;

		std::map<QString, QMetaType::Type> value_type_map;

	signals:
		void settings_changed();

	public slots:
		void editSettings();
};

#endif 

