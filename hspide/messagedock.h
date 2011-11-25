#include <QTreeView>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CMessageDock
	: public QWidget
{
	Q_OBJECT

	QTreeView * listWidget;

public:

	CMessageDock(QWidget *parent = 0);

	void clear();
	void addMessage(const QString & fileName, int lineNum, const QString & description);

protected:

	virtual void resizeEvent(QResizeEvent * event);

private:

protected slots:

	void doubleClickedList(const QModelIndex & inde);

signals:

//	void oepnItem(CWorkSpaceItem * item);

};
