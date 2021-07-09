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
#include "toolkit_config.h"

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

	public slots:
		void editSetting(QTreeWidgetItem *item, int column);

	signals:
		void settings_changed();
};


class TOOLKIT_DLLAPI ToolkitSettings : public QObject{
	Q_OBJECT
	public:
		ToolkitSettings();

		TOOLKIT_DLLAPI QVariant value(const QString &key);
		TOOLKIT_DLLAPI QMetaType::Type type(const QString &key);
		TOOLKIT_DLLAPI void setValue(const QString &key, const QVariant &value);
		TOOLKIT_DLLAPI void setType(const QString &key, const QVariant infer_from_value);

		TOOLKIT_DLLAPI QString group() { return settings.group(); }

		TOOLKIT_DLLAPI void beginGroup(QString group);
		TOOLKIT_DLLAPI void endGroup();

		TOOLKIT_DLLAPI QStringList childKeys() { return settings.childKeys(); }
		TOOLKIT_DLLAPI QStringList childGroups() { return settings.childGroups(); }

	private:
		QSettings settings;

		std::map<QString, QMetaType::Type> value_type_map;

	signals:
		void settings_changed();

	public slots:
		TOOLKIT_DLLAPI void editSettings();
};

#endif 

