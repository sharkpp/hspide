#include <QTextEdit>
#include <QTreeView>
#include <QMap>
#include <QVector>
#include "ipc_common.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class QStandardItem;

class CVariableInfoDock
	: public QWidget
{
	Q_OBJECT

	typedef enum {
		NameColumn = 0,
		TypeColumn,
		DescriptionColumn,
		ColumnCount,
	} ColumnType;

	typedef struct INFO_TYPE {
		QString typeName;
	};

	QTreeView * m_listWidget;

	QMap<qlonglong, VARIABLE_INFO_TYPE> m_varInfo;

	qlonglong m_keyBase;

public:

	CVariableInfoDock(QWidget *parent = 0);

	void setEnable(bool enable);

	void clear();

	void update(const VARIABLE_INFO_TYPE & varInfo);

protected:

	QStandardItem* getItem(const QString & valueName);

	void setItem(QStandardItem* item, ColumnType type, const QString & value);

	QString toPath(const VARIABLE_INFO_TYPE& varInfo);

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

	void onTreeExpanded(const QModelIndex& index);

signals:

	void requestVariableInfo(const QString& varName, int* index);
};
