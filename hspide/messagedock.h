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

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

private:

};
