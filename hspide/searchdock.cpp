#include "searchdock.h"
#include <QResizeEvent>

CSearchDock::CSearchDock(QWidget *parent)
	: QWidget(parent)
{
	mEditor = new QTextEdit(this);
	mEditor->setReadOnly(true);
}

void CSearchDock::resizeEvent(QResizeEvent * event)
{
	mEditor->resize(event->size());
}

