#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QString>

#pragma once

#ifndef INCLUDE_GUARD_3ED30DA5_1A87_433A_9ED9_FCEC987B4EAA
#define INCLUDE_GUARD_3ED30DA5_1A87_433A_9ED9_FCEC987B4EAA

class CCodeEdit
	: public QPlainTextEdit
{
	Q_OBJECT

	friend class CLineNumberArea;

	QWidget *            mLineNumber;
	QSyntaxHighlighter * mHighlighter;

public:

	CCodeEdit(QWidget *parent = 0);

protected:

	virtual void resizeEvent(QResizeEvent * event);

	void paintLineNumEvent(QPaintEvent * event);
	void mousePressLineNumEvent(QMouseEvent * event);

public slots:

private:

};

#endif // !defined(INCLUDE_GUARD_3ED30DA5_1A87_433A_9ED9_FCEC987B4EAA)
