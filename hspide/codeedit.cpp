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

	// シンボル一覧を指定
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
				// 定義済みマクロ
				format = macroFormat;
			} else if( !category2.compare("func") ) {
				if( !category1.compare("pre") ) {
					// プリプロセッサ
					pattern = symbol[0];
					pattern = "(^|\\s)#\\s*" + pattern.remove("#") + "\\b";
					format = preprocesserFormat;
				} else {
					// 関数/命令
					format = functionFormat;
				}
			}
			rule.pattern = QRegExp(pattern);
			rule.format  = format;
			highlightingRules.append(rule);
		}

		// ラベル
		QTextCharFormat labelFormat;
		labelFormat.setForeground(Qt::darkYellow);
		rule.pattern = QRegExp("\\*[^\\s,.:;\\/()=0-9][^\\s,.:;\\/]*");
		rule.format = labelFormat;
		highlightingRules.append(rule);

		// コメント(一行)
		QTextCharFormat singleLineCommentFormat;
		singleLineCommentFormat.setForeground(Qt::darkGreen);
		rule.pattern = QRegExp("//[^\n]*");
		rule.format = singleLineCommentFormat;
		highlightingRules.append(rule);
		rule.pattern = QRegExp(";[^\n]*");
		rule.format = singleLineCommentFormat;
		highlightingRules.append(rule);

		// コメント(複数行)
		multiLineCommentFormat.setForeground(Qt::darkGreen);

		// 文字列
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
	, m_lineNumberBackgroundColorRole(QPalette::Window)
	, m_lineNumberTextColorRole(QPalette::Text)
	, m_lineNumberBackgroundColor(QColor(255, 255, 255, 255))
	, m_lineNumberTextColor(QColor(255, 0, 0, 0))
	, m_visibleLineNumber(true)
	, m_lineIconSize(16, 16)
{
	m_lineNumberWidget  = new CLineNumberArea(this);
	m_highlighter = new CHighlighter(document());

	connect(this, SIGNAL(blockCountChanged(int)),   this, SLOT(updateLineNumberWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumber(QRect,int)));

	updateLineNumberWidth(0);
	
setLineIcon(0, QIcon(":/images/tango/small/dialog-error.png"));
}

// シンボル一覧を指定
void CCodeEdit::setSymbols(const QVector<QStringList> & symbols)
{
	static_cast<CHighlighter*>(m_highlighter)->setSymbols(symbols);
}

int CCodeEdit::lineNumberWidth()
{
	int drawWidth = 0;

	drawWidth += m_lineIconSize.width();

	if( m_visibleLineNumber )
	{
		int digits = 1;
		for(int n = qMax(10, blockCount());
			10 <= n; digits++)
		{
			n /= 10;
		}

		drawWidth += 3 + fontMetrics().width(QLatin1Char('9')) * digits;
		drawWidth += fontMetrics().width(QLatin1Char('#'));
	}

	return drawWidth;
}

//////////////////////////////////////////////////////////////////////
// プロパティ
//////////////////////////////////////////////////////////////////////

void CCodeEdit::setLineNumberBackgroundColorRole(const QPalette::ColorRole & role)
{
	m_lineNumberBackgroundColorRole = role;
}

void CCodeEdit::setLineNumberTextColorRole(const QPalette::ColorRole & role)
{
	m_lineNumberTextColorRole = role;
}

void CCodeEdit::setLineNumberBackgroundColor(const QColor & color)
{
	m_lineNumberBackgroundColor = color;
	m_lineNumberBackgroundColorRole = QPalette::NoRole;
}

inline void CCodeEdit::setLineNumberTextColor(const QColor & color)
{
	m_lineNumberTextColor = color;
	m_lineNumberTextColorRole = QPalette::NoRole;
}

inline void CCodeEdit::setLineNumberVisible(bool visible)
{
	m_visibleLineNumber = visible;

	if( visible != m_lineNumberWidget->isVisible() )
	{
		if( visible ) {
			m_lineNumberWidget->show();
		} else {
			m_lineNumberWidget->hide();
		}
	}
}

void CCodeEdit::setLineIcon(int lineNo, const QIcon & icon)
{
	m_lineIconMap[lineNo] = icon;
}

const QIcon & CCodeEdit::lineIcon(int lineNo)
{
	QMap<int, QIcon>::iterator
		ite = m_lineIconMap.find(lineNo);
	if( m_lineIconMap.end() != ite )
	{
		return ite.value();
	}
	return QIcon();
}

void CCodeEdit::clearLineIcon()
{
	m_lineIconMap.clear();
}

void CCodeEdit::clearLineIcon(const QIcon & icon)
{
	qint64 iconHash = icon.cacheKey();

	for(QMap<int, QIcon>::iterator
			ite = m_lineIconMap.begin();
		ite != m_lineIconMap.end(); )
	{
		if( iconHash == ite.value().cacheKey() )
		{
			m_lineIconMap.erase(ite++);
		}
		else
		{
			++ite;
		}
	}
}

void CCodeEdit::clearLineIcon(int lineNo)
{
	QMap<int, QIcon>::iterator
		ite = m_lineIconMap.find(lineNo);
	if( m_lineIconMap.end() != ite )
	{
		m_lineIconMap.erase(ite);
	}
}

//////////////////////////////////////////////////////////////////////
// イベント
//////////////////////////////////////////////////////////////////////

void CCodeEdit::updateLineNumberWidth(int newBlockCount)
{
	setViewportMargins(lineNumberWidth(), 0, 0, 0);
}

void CCodeEdit::updateLineNumber(const QRect & rect ,int dy)
{
	m_lineNumberWidget->update(0, rect.y(), m_lineNumberWidget->width(), rect.height());
}

void CCodeEdit::resizeEvent(QResizeEvent * event)
{
	QPlainTextEdit::resizeEvent(event);

	const QRect & rc = contentsRect();
	m_lineNumberWidget->setGeometry(rc.left(), rc.top(), lineNumberWidth(), rc.height());
}

void CCodeEdit::paintLineNumEvent(QPaintEvent * event)
{
	QPainter painter(m_lineNumberWidget);

	if( !m_visibleLineNumber )
	{
		return;
	}

	// 行番号エリアをベタ塗り
	painter.fillRect(event->rect(), lineNumberBackgroundColor());

	// 行番号を描画
	painter.setPen(lineNumberTextColor());
	QTextBlock block = firstVisibleBlock();
	int lineHeight = fontMetrics().height();
	int y = event->rect().y();
	int lineNumWidth = lineNumberWidth();
	int lineNumSpace = fontMetrics().width(QLatin1Char('#')) / 2;
	for(int lineNo = block.blockNumber();
		block.isValid() && y <= event->rect().bottom();
		lineNo++, block = block.next())
	{
		// 行番号を描画
		QRectF rect = blockBoundingGeometry(block).translated(contentOffset());
		y = (int)rect.top();
		painter.drawText(0, y,
		                 lineNumWidth - lineNumSpace, lineHeight,
		                 Qt::AlignRight, QString::number(lineNo + 1));
		// アイコンを描画
		QMap<int, QIcon>::iterator
			ite = m_lineIconMap.find(lineNo);
		if( m_lineIconMap.end() != ite )
		{
			QPixmap pixmap = ite.value().pixmap(m_lineIconSize, QIcon::Normal);
			painter.drawPixmap(QPoint(0, y + int(rect.height() - m_lineIconSize.height())), pixmap);
		}
	}
}

void CCodeEdit::mousePressLineNumEvent(QMouseEvent * event)
{
	QPoint pt(0, event->y());
	QTextCursor cursor = cursorForPosition(pt);
	setTextCursor(cursor);
}
