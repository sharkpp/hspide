#include "editorwidget.h"
#include <QtGui>

EditorWidget::EditorWidget(QWidget *parent)
	: QWidget(parent)
{
	textEditor = new QTextEdit(this);
}

void EditorWidget::setPlainText(const QString &text)
{
	textEditor->setPlainText(text);
}

