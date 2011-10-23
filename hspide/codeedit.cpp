#include <QtGui>
#include "codeedit.h"

//////////////////////////////////////////////////////////////////////

class CLineNumberArea
	: public QWidget
{
public:

	CLineNumberArea(QWidget *parent)
		: QWidget(parent)
	{
	}

	virtual ~CLineNumberArea()
	{
	}

public:

	virtual void paintEvent(QPaintEvent * event)
	{
		static_cast<CCodeEdit*>(parentWidget())->paintLineNumEvent(event);
	}

	virtual void mousePressEvent(QMouseEvent * event)
	{
		static_cast<CCodeEdit*>(parentWidget())->mousePressLineNumEvent(event);
	}

};

//////////////////////////////////////////////////////////////////////

class CHighlighter
	: public QSyntaxHighlighter
{
private:


public:

	CHighlighter(QTextDocument *parent)
		: QSyntaxHighlighter(parent)
	{
	}

	virtual ~CHighlighter()
	{
	}

private:

	virtual void highlightBlock(const QString &text)
	{
		QTextCharFormat myClassFormat;
		myClassFormat.setFontWeight(QFont::Bold);
		myClassFormat.setForeground(Qt::darkMagenta);
		QString pattern = "\\bMy[A-Za-z]+\\b";

		QRegExp expression(pattern);
		int index = text.indexOf(expression);
		while (index >= 0) {
			int length = expression.matchedLength();
			setFormat(index, length, myClassFormat);
			index = text.indexOf(expression, index + length);
		}

		QTextCharFormat multiLineCommentFormat;
		multiLineCommentFormat.setForeground(Qt::red);

		QRegExp startExpression("/\\*");
		QRegExp endExpression("\\*/");

		setCurrentBlockState(0);

		int startIndex = 0;
		if (previousBlockState() != 1)
			startIndex = text.indexOf(startExpression);

		while (startIndex >= 0) {
			int endIndex = text.indexOf(endExpression, startIndex);
			int commentLength;
			if (endIndex == -1) {
				setCurrentBlockState(1);
				commentLength = text.length() - startIndex;
			} else {
				commentLength = endIndex - startIndex
				              + endExpression.matchedLength();
			}
			setFormat(startIndex, commentLength, multiLineCommentFormat);
			startIndex = text.indexOf(startExpression,
			                          startIndex + commentLength);
		}
	}

};

//////////////////////////////////////////////////////////////////////


CCodeEdit::CCodeEdit(QWidget *parent)
	: QPlainTextEdit(parent)
{
	mLineNumber = new CLineNumberArea(this);

	mHighlighter = new CHighlighter(document());

	setViewportMargins(100, 0, 0, 0);
}

void CCodeEdit::resizeEvent(QResizeEvent * event)
{
//	mLineNumber->resize(event->size());
	mLineNumber->setGeometry(0, 0, 100, event->size().height());
}

void CCodeEdit::paintLineNumEvent(QPaintEvent * event)
{
	QPainter painter(mLineNumber);
	painter.fillRect(event->rect(), Qt::lightGray);
}

void CCodeEdit::mousePressLineNumEvent(QMouseEvent * event)
{
	QPoint pt(0, event->y());
	QTextCursor cursor = cursorForPosition(pt);
	setTextCursor(cursor);
}
