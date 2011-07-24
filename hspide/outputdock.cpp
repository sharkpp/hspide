#include "outputdock.h"
#include <QResizeEvent>
#include <QTextCursor>

COutputDock::COutputDock(QWidget *parent)
	: QWidget(parent)
{
	mEditor = new QTextEdit(this);
	mEditor->setReadOnly(true);
}

void COutputDock::resizeEvent(QResizeEvent * event)
{
	mEditor->resize(event->size());
}

// o—Í‚É•¶Žš—ñ‚ð’Ç‹L
void COutputDock::output(const QString & text)
{
	QTextCursor cursor(mEditor->document());
	cursor.movePosition(QTextCursor::End);
	cursor.insertText(text + "\n");
}
