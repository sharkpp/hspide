#include "varinfodock.h"
#include <QResizeEvent>

CVariableInfoDock::CVariableInfoDock(QWidget *parent)
	: QWidget(parent)
{
	editorWidget = new QTextEdit(this);
	editorWidget->setReadOnly(true);
}

void CVariableInfoDock::resizeEvent(QResizeEvent * event)
{
	editorWidget->resize(event->size());
}

