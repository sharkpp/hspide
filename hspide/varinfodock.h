#include <QTextEdit>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CVariableInfoDock
	: public QWidget
{
	Q_OBJECT

	QTextEdit * editorWidget;

public:

	CVariableInfoDock(QWidget *parent = 0);

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

private:

};
