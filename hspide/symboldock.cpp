#include "symboldock.h"
#include <QResizeEvent>

CSymbolDock::CSymbolDock(QWidget *parent)
	: QWidget(parent)
{
	mEditor = new QTextEdit(this);
	mEditor->setReadOnly(true);
}

void CSymbolDock::resizeEvent(QResizeEvent * event)
{
	mEditor->resize(event->size());
}

