#include <QWidget>
#include <QUuid>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class QResizeEvent;
class QTreeWidget;
class QTreeWidgetItem;

class CBreakPointDock
	: public QWidget
{
	Q_OBJECT

	typedef enum {
		FileColumn = 0,
		ColumnCount,
	} ColumnType;

	QTreeWidget * m_listWidget;

public:

	CBreakPointDock(QWidget *parent = 0);

	void setEnable(bool enable);

protected:

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

	void onBreakPointChanged(const QUuid& uuid, const QList<QPair<int, bool> >& modifiedLineNumberes);
	void onBreakPointStateChanged(QTreeWidgetItem* item, int column);
	void onBreakPointDoubleClicked(QTreeWidgetItem* item, int column);

signals:

	void gotoLine(const QUuid & uuid, int lineNo);

};
