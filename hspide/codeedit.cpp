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
	, mLineNumberBackgroundColorRole(QPalette::Window)
	, mLineNumberTextColorRole(QPalette::Text)
	, mLineNumberBackgroundColor(QColor(255, 255, 255, 255))
	, mLineNumberTextColor(QColor(255, 0, 0, 0))
{
	mLineNumber = new CLineNumberArea(this);

	mHighlighter = new CHighlighter(document());

	connect(this, SIGNAL(blockCountChanged(int)),   this, SLOT(updateLineNumberWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumber(QRect,int)));

	updateLineNumberWidth(0);
}

int CCodeEdit::lineNumberWidth()
{
	int digits = 1;
	for(int n = qMax(10, blockCount());
		10 <= n; digits++)
	{
		n /= 10;
	}

	int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
	    space+= fontMetrics().width(QLatin1Char('#'));

	return space;
}

void CCodeEdit::updateLineNumberWidth(int newBlockCount)
{
	setViewportMargins(lineNumberWidth(), 0, 0, 0);
}

void CCodeEdit::updateLineNumber(const QRect & rect ,int dy)
{
	//if( rect.contains(mLineNumber->geometry()) ) 
	//{
	//}
	//if( dy ) {
	//	mLineNumber->scroll(0, dy);
	//} else {
		mLineNumber->update(0, rect.y(), mLineNumber->width(), rect.height());
	//}
}

void CCodeEdit::resizeEvent(QResizeEvent * event)
{
	QPlainTextEdit::resizeEvent(event);

	const QRect & rc = contentsRect();
	mLineNumber->setGeometry(rc.left(), rc.top(), lineNumberWidth(), rc.height());
}

void CCodeEdit::paintLineNumEvent(QPaintEvent * event)
{
	QPainter painter(mLineNumber);

	// 行番号エリアをベタ塗り
	painter.fillRect(event->rect(), lineNumberBackgroundColor());

	// 行番号を描画
	painter.setPen(lineNumberTextColor());
	QTextBlock block = firstVisibleBlock();
	int lineHeight = fontMetrics().height();
	int y = event->rect().y();
	int lineNumWidth = lineNumberWidth();
	int lineNumSpace = fontMetrics().width(QLatin1Char('#')) / 2;
	for(int lineNum = block.blockNumber();
		block.isValid() && y <= event->rect().bottom();
		lineNum++, block = block.next())
	{
		y = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
		painter.drawText(0, y, lineNumWidth - lineNumSpace, lineHeight, Qt::AlignRight, QString::number(lineNum + 1));
	}
}

void CCodeEdit::mousePressLineNumEvent(QMouseEvent * event)
{
	QPoint pt(0, event->y());
	QTextCursor cursor = cursorForPosition(pt);
	setTextCursor(cursor);
}
