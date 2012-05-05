#include <QTextEdit>
#include <QTreeView>
#include <QMap>
#include <QVector>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class QStandardItem;

class CBreakPointDock
	: public QWidget
{
	Q_OBJECT

	typedef enum {
		FileColumn = 0,
		ColumnCount,
	} ColumnType;

	QTreeView * m_listWidget;

public:

	CBreakPointDock(QWidget *parent = 0);

	void setEnable(bool enable);

protected:

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

signals:

};
