#include "projectdock.h"
#include <QResizeEvent>

CProjectDock::CProjectDock(QWidget *parent)
	: QWidget(parent)
{
	mEditor = new QTextEdit(this);
	mEditor->setReadOnly(true);
}

void CProjectDock::resizeEvent(QResizeEvent * event)
{
	mEditor->resize(event->size());
}

