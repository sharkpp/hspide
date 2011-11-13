#include "outputdock.h"
#include <QResizeEvent>
#include <QTextCursor>

COutputDock::COutputDock(QWidget *parent)
	: QWidget(parent)
{
	editorWidget = new QTextEdit(this);
	editorWidget->setReadOnly(true);
}

void COutputDock::resizeEvent(QResizeEvent * event)
{
	editorWidget->resize(event->size());
}

// �o�͂��N���A
void COutputDock::clear()
{
	editorWidget->clear();
}

// �o�͂ɕ������ǋL
void COutputDock::output(const QString & text)
{
	QTextCursor cursor(editorWidget->document());
	cursor.movePosition(QTextCursor::End);
	cursor.insertText(text);
}

void COutputDock::outputCrLf(const QString & text)
{
	output(text + "\n");
}
