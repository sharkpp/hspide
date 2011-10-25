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

	struct HighlightingRule
	{
		QRegExp pattern;
		QTextCharFormat format;
	};
	QVector<HighlightingRule> highlightingRules;

	QRegExp commentStartExpression;
	QRegExp commentEndExpression;

	QTextCharFormat multiLineCommentFormat;

public:

	CHighlighter(QTextDocument *parent)
		: QSyntaxHighlighter(parent)
	{
		commentStartExpression = QRegExp("/\\*");
		commentEndExpression = QRegExp("\\*/");
	}

	virtual ~CHighlighter()
	{
	}

	// �V���{���ꗗ���w��
	void setSymbols(const QVector<QStringList> & symbols)
	{
		HighlightingRule rule;

		highlightingRules.clear();

		QTextCharFormat functionFormat;
		functionFormat.setForeground(Qt::blue);

		QTextCharFormat preprocesserFormat;
		preprocesserFormat.setForeground(Qt::blue);

		QTextCharFormat macroFormat;
		macroFormat.setForeground(Qt::blue);

		foreach(const QStringList & symbol, symbols)
		{
			const QString & category1 = symbol[1];
			const QString & category2 = symbol[2];
			QString pattern = "\\b" + symbol[0] + "\\b";
			QTextCharFormat format;
			if( !category2.compare("macro") ) {
				// ��`�ς݃}�N��
				format = macroFormat;
			} else if( !category2.compare("func") ) {
				if( !category1.compare("pre") ) {
					// �v���v���Z�b�T
					pattern = symbol[0];
					pattern = "(^|\\s)#\\s*" + pattern.remove("#") + "\\b";
					format = preprocesserFormat;
				} else {
					// �֐�/����
					format = functionFormat;
				}
			}
			rule.pattern = QRegExp(pattern);
			rule.format  = format;
			highlightingRules.append(rule);
		}

		// ���x��
		QTextCharFormat labelFormat;
		labelFormat.setForeground(Qt::darkYellow);
		rule.pattern = QRegExp("\\*[^\\s,.:;\\/()=0-9][^\\s,.:;\\/]*");
		rule.format = labelFormat;
		highlightingRules.append(rule);

		// �R�����g(��s)
		QTextCharFormat singleLineCommentFormat;
		singleLineCommentFormat.setForeground(Qt::darkGreen);
		rule.pattern = QRegExp("//[^\n]*");
		rule.format = singleLineCommentFormat;
		highlightingRules.append(rule);
		rule.pattern = QRegExp(";[^\n]*");
		rule.format = singleLineCommentFormat;
		highlightingRules.append(rule);

		// �R�����g(�����s)
		multiLineCommentFormat.setForeground(Qt::darkGreen);

		// ������
		QTextCharFormat quotationFormat;
		quotationFormat.setForeground(Qt::darkRed);
		rule.pattern = QRegExp("\".*\"");
		rule.format = quotationFormat;
		highlightingRules.append(rule);

	}

private:

	virtual void highlightBlock(const QString &text)
	{
		foreach (const HighlightingRule &rule, highlightingRules) {
			QRegExp expression(rule.pattern);
			int index = expression.indexIn(text);
			while (index >= 0) {
				int length = expression.matchedLength();
				setFormat(index, length, rule.format);
				index = expression.indexIn(text, index + length);
			}
		}
		setCurrentBlockState(0);

		int startIndex = 0;
		if (previousBlockState() != 1)
			startIndex = commentStartExpression.indexIn(text);

		while (startIndex >= 0) {
			int endIndex = commentEndExpression.indexIn(text, startIndex);
			int commentLength;
			if (endIndex == -1) {
				setCurrentBlockState(1);
				commentLength = text.length() - startIndex;
			} else {
				commentLength = endIndex - startIndex
								+ commentEndExpression.matchedLength();
			}
			setFormat(startIndex, commentLength, multiLineCommentFormat);
			startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
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
	mLineNumber  = new CLineNumberArea(this);
	mHighlighter = new CHighlighter(document());

	connect(this, SIGNAL(blockCountChanged(int)),   this, SLOT(updateLineNumberWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumber(QRect,int)));

	updateLineNumberWidth(0);
}

// �V���{���ꗗ���w��
void CCodeEdit::setSymbols(const QVector<QStringList> & symbols)
{
	static_cast<CHighlighter*>(mHighlighter)->setSymbols(symbols);
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
	mLineNumber->update(0, rect.y(), mLineNumber->width(), rect.height());
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

	// �s�ԍ��G���A���x�^�h��
	painter.fillRect(event->rect(), lineNumberBackgroundColor());

	// �s�ԍ���`��
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