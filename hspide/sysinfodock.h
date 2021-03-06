#include <QTreeView>
#include <QMap>
#include <QVector>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class QStandardItem;

class CSystemInfoDock
	: public QWidget
{
	Q_OBJECT

	typedef enum {
		NameColumn = 0,
		DescriptionColumn,
		ColumnCount,
	} ColumnType;

	typedef struct INFO_TYPE {
		QString typeName;
	};

	QTreeView* m_listWidget;

	QMap<QVector<QString>, INFO_TYPE> m_varInfo;

public:

	CSystemInfoDock(QWidget *parent = 0);

	void setEnable(bool enable);

	void clear();

	void update(const QString& valueName, const QString& description);

protected:

	QStandardItem* getItem(const QString& valueName);

	void setItem(QStandardItem* item, ColumnType type, const QString& value);

protected:

	virtual void resizeEvent(QResizeEvent* event);

public slots:

};
