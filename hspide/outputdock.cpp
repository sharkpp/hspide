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

// 出力をクリア
void COutputDock::clear()
{
	mEditor->clear();
}

// 出力に文字列を追記
void COutputDock::output(const QString & text)
{
	QTextCursor cursor(mEditor->document());
	cursor.movePosition(QTextCursor::End);
	cursor.insertText(text);
}

void COutputDock::outputCrLf(const QString & text)
{
	output(text + "\n");
}
