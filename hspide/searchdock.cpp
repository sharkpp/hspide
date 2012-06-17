#include "searchdock.h"
#include <QResizeEvent>

CSearchDock::CSearchDock(QWidget *parent)
	: QWidget(parent)
{
	editorWidget = new QTextEdit(this);
	editorWidget->setReadOnly(true);
}

void CSearchDock::resizeEvent(QResizeEvent* event)
{
	editorWidget->resize(event->size());
}

