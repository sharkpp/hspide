#include "debuggerdock.h"
#include <QResizeEvent>

CDebuggerDock::CDebuggerDock(QWidget *parent)
	: QWidget(parent)
{
	mEditor = new QTextEdit(this);
	mEditor->setReadOnly(true);
}

void CDebuggerDock::resizeEvent(QResizeEvent * event)
{
	mEditor->resize(event->size());
}

