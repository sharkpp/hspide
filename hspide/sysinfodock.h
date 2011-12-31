#include <QTextEdit>
#include <QTreeView>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CSystemInfoDock
	: public QWidget
{
	Q_OBJECT

	QTreeView * listWidget;

public:

	CSystemInfoDock(QWidget *parent = 0);

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

private:

};
