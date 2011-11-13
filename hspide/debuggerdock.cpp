#include "debuggerdock.h"
#include <QResizeEvent>

CDebuggerDock::CDebuggerDock(QWidget *parent)
	: QWidget(parent)
{
	editorWidget = new QTextEdit(this);
	editorWidget->setReadOnly(true);
}

void CDebuggerDock::resizeEvent(QResizeEvent * event)
{
	editorWidget->resize(event->size());
}

