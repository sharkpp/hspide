#include <QtGui>
#include <QDebug>
#include "codeedit.h"
#include "hsp3lexer.h"

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

	virtual void mouseReleaseEvent(QMouseEvent * event)
	{
		static_cast<CCodeEdit*>(parentWidget())->mouseReleaseLineNumEvent(event);
	}

	virtual void mouseMoveEvent(QMouseEvent * event)
	{
		static_cast<CCodeEdit*>(parentWidget())->mouseMoveLineNumEvent(event);
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

		QMap<int, QString> categories;

		foreach(const QStringList & symbol, symbols)
		//{
		//	const QString & category1 = symbol[1];
		//	const QString & category2 = symbol[2];
		//	QString pattern = "(\\b" + symbol[0] + "\\b)";
		//	QTextCharFormat format;
		//	int category = -1;
		//	if( !category2.compare("macro") ) {
		//		// 定義済みマクロ
		//	//	format = macroFormat;
		//		category = 0;
		//	} else if( !category2.compare("func") ) {
		//		if( !category1.compare("pre") ) {
		//			// プリプロセッサ
		//			pattern = symbol[0];
		//			pattern = "((^|\\s)#\\s*" + pattern.remove("#") + "\\b)";
		//		//	format = preprocesserFormat;
		//			category = 1;
		//		} else {
		//			// 関数/命令
		//		//	format = functionFormat;
		//			category = 2;
		//		}
		//	}
		//	categories[category] += (categories[category].isEmpty() ? "" : "|") + pattern;
		//}
		//foreach(int category, categories.keys())
		//{
		//	QTextCharFormat format;
		//	switch(category) {
		//	case 0: format = macroFormat; break;
		//	case 1: format = preprocesserFormat; break;
		//	case 2: format = functionFormat; break;
		//	}
		//	rule.pattern = QRegExp(categories[category]);
		//	rule.format  = format;
		//	highlightingRules.append(rule);
		//}
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

		//// ラベル
		//QTextCharFormat labelFormat;
		//labelFormat.setForeground(Qt::darkYellow);
		//rule.pattern = QRegExp("\\*[^\\s,.:;\\/()=0-9][^\\s,.:;\\/]*");
		//rule.format = labelFormat;
		//highlightingRules.append(rule);

		//// コメント(一行)
		//QTextCharFormat singleLineCommentFormat;
		//singleLineCommentFormat.setForeground(Qt::darkGreen);
		//rule.pattern = QRegExp("//[^\n]*");
		//rule.format = singleLineCommentFormat;
		//highlightingRules.append(rule);
		//rule.pattern = QRegExp(";[^\n]*");
		//rule.format = singleLineCommentFormat;
		//highlightingRules.append(rule);

		//// コメント(複数行)
		//multiLineCommentFormat.setForeground(Qt::darkGreen);

		//// 文字列
		//QTextCharFormat quotationFormat;
		//quotationFormat.setForeground(Qt::darkRed);
		//rule.pattern = QRegExp("\".*\"");
		//rule.format = quotationFormat;
		//highlightingRules.append(rule);

	}

private:

	typedef enum {
		HSP3_TOKEN_NORMAL = 0,
		HSP3_TOKEN_STRING,
		HSP3_TOKEN_LABEL,
		HSP3_TOKEN_COMMENT,
	} HSP3_TOKEN_TYPE;

	typedef struct HSP3_TOKEN {
		QStringRef text;
		int        start;
		int        length;
		HSP3_TOKEN_TYPE type;
	} HSP3_TOKEN;

	virtual void highlightBlock(const QString& text)
	{
		//QRegExp expression("\\b.+\\b");
		//QTextCharFormat preprocesserFormat;
		//preprocesserFormat.setForeground(Qt::blue);
		//int index = expression.indexIn(text);
		//while (index >= 0) {
		//	int length = expression.matchedLength();
		//	setFormat(index, length, preprocesserFormat);
		//	index = expression.indexIn(text, index + length);
		//}

		Hsp3Lexer lexer;

		lexer.reset(QString(text).append(QChar::LineSeparator), previousBlockState());

		QTextCharFormat testFormat;
		testFormat.setForeground(Qt::red);
		testFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);

		QTextCharFormat format[Hsp3Lexer::TypeNum];

	//	// プリプロセッサ
	//	QTextCharFormat preprocesserFormat;
	//	preprocesserFormat.setForeground(Qt::blue);
	//	preprocesserFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);

		// ラベル
		QTextCharFormat &labelFormat = format[Hsp3Lexer::TypeLabel];
		labelFormat.setForeground(Qt::darkYellow);
	//	labelFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);

		// コメント
		QTextCharFormat &commentFormat = format[Hsp3Lexer::TypeComment];
		commentFormat.setForeground(Qt::darkGreen);
	//	commentFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);

		// 文字列
		QTextCharFormat &stringFormat = format[Hsp3Lexer::TypeString];
		stringFormat.setForeground(Qt::darkRed);
	//	stringFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);

		format[Hsp3Lexer::TypeNormal].setUnderlineStyle(QTextCharFormat::SingleUnderline);

		while( lexer.scan() )
		{
			setFormat(lexer.start(), lexer.length(), format[lexer.type()]);
		}

		setCurrentBlockState( lexer.state() );

	//	QTextCharFormat functionFormat;
	//	functionFormat.setForeground(Qt::blue);
	//	functionFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    //
	//	QTextCharFormat preprocesserFormat;
	//	preprocesserFormat.setForeground(Qt::blue);
    //
	//	QTextCharFormat macroFormat;
	//	macroFormat.setForeground(Qt::blue);
    //
	//	foreach(const HSP3_TOKEN& token, tokens) {
	//		setFormat(token.start, token.length - 1, functionFormat);
	//	}

//qDebug() << __LINE__ << QTime::currentTime().toString("hh:mm:ss.zzz") << text;
//	//	foreach (const HighlightingRule &rule, highlightingRules) {
//		for(int i = 0, num = highlightingRules.size(); i < num; i++) {
//			HighlightingRule& rule = highlightingRules[i];
//			QRegExp & expression = rule.pattern;
//		//	QRegExp expression(rule.pattern);
//			int index = expression.indexIn(text);
//			while (index >= 0) {
//				int length = expression.matchedLength();
//				setFormat(index, length, rule.format);
//				index = expression.indexIn(text, index + length);
//			}
//		}
//qDebug() << __LINE__ << QTime::currentTime().toString("hh:mm:ss.zzz");
	//	setCurrentBlockState(0);
    //
	//	int startIndex = 0;
	//	if (previousBlockState() != 1)
	//		startIndex = commentStartExpression.indexIn(text);
    //
	//	while (startIndex >= 0) {
	//		int endIndex = commentEndExpression.indexIn(text, startIndex);
	//		int commentLength;
	//		if (endIndex == -1) {
	//			setCurrentBlockState(1);
	//			commentLength = text.length() - startIndex;
	//		} else {
	//			commentLength = endIndex - startIndex
	//							+ commentEndExpression.matchedLength();
	//		}
	//		setFormat(startIndex, commentLength, multiLineCommentFormat);
	//		startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
	//	}
	}

};

//////////////////////////////////////////////////////////////////////


CCodeEdit::CCodeEdit(QWidget *parent)
	: QPlainTextEdit(parent)
	, m_lineIconBackgroundColorRole(QPalette::Window)
	, m_lineNumberBackgroundColorRole(QPalette::Light)
	, m_lineNumberTextColorRole(QPalette::Text)
	, m_lineIconBackgroundColor(QColor(255, 255, 255, 255))
	, m_lineNumberBackgroundColor(QColor(255, 255, 255, 255))
	, m_lineNumberTextColor(QColor(255, 0, 0, 0))
	, m_visibleLineNumber(true)
	, m_lineIconSize(16, 16)
	, m_tabStopCharWidth(4)
{
	m_lineNumberWidget = new CLineNumberArea(this);
	m_highlighter      = new CHighlighter(document());

	connect(this, SIGNAL(blockCountChanged(int)),   this, SLOT(updateLineNumberWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumber(QRect,int)));

	updateLineNumberWidth(0);

	QFont font_ = font();
	font_.setFamily("MS Gothic");
	font_.setFixedPitch(true);
//	font_.setPixelSize(16);
	setFont(font_);

	// タブストップを設定
	setTabStopCharWidth(m_tabStopCharWidth);
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

void CCodeEdit::setLineIconBackgroundColorRole(const QPalette::ColorRole & role)
{
	m_lineIconBackgroundColorRole = role;
}

void CCodeEdit::setLineNumberBackgroundColorRole(const QPalette::ColorRole & role)
{
	m_lineNumberBackgroundColorRole = role;
}

void CCodeEdit::setLineNumberTextColorRole(const QPalette::ColorRole & role)
{
	m_lineNumberTextColorRole = role;
}

void CCodeEdit::setLineIconBackgroundColor(const QColor & color)
{
	m_lineIconBackgroundColor = color;
	m_lineIconBackgroundColorRole = QPalette::NoRole;
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
	m_lineNumberWidget->update(0, contentsRect().y(), m_lineNumberWidget->width(), contentsRect().height());
}

const QIcon & CCodeEdit::lineIcon(int lineNo)
{
	static QIcon tmp;
	QMap<int, QIcon>::iterator
		ite = m_lineIconMap.find(lineNo);
	if( m_lineIconMap.end() != ite )
	{
		return ite.value();
	}
	return tmp;
}

void CCodeEdit::clearLineIcon()
{
	m_lineIconMap.clear();
	m_lineNumberWidget->update(0, contentsRect().y(), m_lineNumberWidget->width(), contentsRect().height());
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
	m_lineNumberWidget->update(0, contentsRect().y(), m_lineNumberWidget->width(), contentsRect().height());
}

void CCodeEdit::clearLineIcon(int lineNo)
{
	QMap<int, QIcon>::iterator
		ite = m_lineIconMap.find(lineNo);
	if( m_lineIconMap.end() != ite )
	{
		m_lineIconMap.erase(ite);
	}
	m_lineNumberWidget->update(0, contentsRect().y(), m_lineNumberWidget->width(), contentsRect().height());
}

void CCodeEdit::setTabStopCharWidth(int width)
{
	m_tabStopCharWidth = width;
	setTabStopWidth(fontMetrics().width(QLatin1Char(' ')) * width);
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

void CCodeEdit::del()
{
	QTextCursor cursor = textCursor();
	cursor.deleteChar();
}

void CCodeEdit::keyPressEvent(QKeyEvent* event)
{
	QString text = event->text();

	if( !text.isEmpty() &&
		"\t" == text ) 
	{
		QTextCursor cursor = textCursor();
		int startSel = cursor.selectionStart();
		int endSel   = cursor.selectionEnd();
	//	cursor.setPosition(startSel, QTextCursor::KeepAnchor);
	//	int startLine = cursor.blockNumber();
	//	cursor.setPosition(endSel, QTextCursor::KeepAnchor);
	//	int startEnd  = cursor.blockNumber();
		cursor.setPosition(startSel);
		cursor.movePosition(QTextCursor::StartOfLine);
		cursor.setPosition(endSel - 1, QTextCursor::KeepAnchor);
		cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

		startSel = cursor.selectionStart();
		endSel   = cursor.selectionEnd();

		QStringList lines(cursor.selectedText().split(QChar::ParagraphSeparator));
		if( !lines.isEmpty() )
		{
			bool skipLastLine = lines[lines.size() - 1].isEmpty();
			if( skipLastLine ) {
				lines.removeLast();
			}
			for(int i = 0, num = lines.size(); i < num; i++, endSel++) {
				lines[i].insert(0, "\t");
			}
			if( skipLastLine ) {
				lines.append("");
			}
		}
		QString text = lines.join(QString(QChar::ParagraphSeparator));

qDebug() << __LINE__ << QTime::currentTime().toString("hh:mm:ss.zzz");
		cursor.beginEditBlock();
		cursor.insertText(text);
		cursor.endEditBlock();
qDebug() << __LINE__ << QTime::currentTime().toString("hh:mm:ss.zzz");

		cursor.setPosition(startSel);
		cursor.movePosition(QTextCursor::StartOfLine);
		cursor.setPosition(endSel - 1, QTextCursor::KeepAnchor);
		cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);

		setTextCursor(cursor);

		return;
	}

	QPlainTextEdit::keyPressEvent(event);
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
	const QRect & rc = event->rect();
	painter.fillRect(rc, lineNumberBackgroundColor());
	painter.fillRect(QRect(	rc.left(), rc.top(),
							rc.left() + m_lineIconSize.width(),
							rc.height()), lineIconBackgroundColor());

	// 行番号との区切りを描画
	const QRect & rcWnd = contentsRect();
	QPen pen;
	pen.setColor(lineNumberTextColor());
	pen.setStyle(Qt::DotLine);
	painter.setPen(pen);
	// ※クリックしたりするとごみが描画されるので毎回、上から下までを描画する
	painter.fillRect(QRect(	rc.right(), rcWnd.top(), rc.right(), rcWnd.height()),
					 lineIconBackgroundColor());
	painter.drawLine(rc.right(), rcWnd.top(), rc.right(), rcWnd.bottom());

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
			QPixmap pixmap = ite.value().pixmap(QSize(m_lineIconSize.width() - 2, m_lineIconSize.height() - 2), QIcon::Normal);
			painter.drawPixmap(QPoint(1, y + int(rect.height() - m_lineIconSize.height()) + 1), pixmap);
		}
	}
}

void CCodeEdit::mousePressLineNumEvent(QMouseEvent * event)
{
	if( m_lineIconSize.width() < event->x() )
	{
		QPoint pt(0, event->y());
		QTextCursor cursor = cursorForPosition(pt);
		cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		setTextCursor(cursor);
	}
	else
	{
		// アイコン部分がクリックされた
		QPoint pt(0, event->y());
		QTextCursor cursor = cursorForPosition(pt);
		emit pressIconArea(cursor.blockNumber());
	}
}

void CCodeEdit::mouseReleaseLineNumEvent(QMouseEvent * event)
{
}

void CCodeEdit::mouseMoveLineNumEvent(QMouseEvent * event)
{
	QPoint pt(0, event->y());

	if( m_lineIconSize.width() < event->x() &&
		0 != (event->buttons() & Qt::LeftButton) )
	{
		QTextCursor cursor = textCursor();
		if( cursor.position() == cursor.selectionStart() )
		{
			// 選択開始位置より前を選択
			cursor.setPosition(cursor.selectionEnd());
			cursor.movePosition(QTextCursor::EndOfLine);
			cursor.setPosition(cursorForPosition(pt).position(), QTextCursor::KeepAnchor);
			cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
		}
		else
		{
			// 選択開始位置より後ろを選択
			cursor.setPosition(cursor.selectionStart());
			cursor.movePosition(QTextCursor::StartOfLine);
			cursor.setPosition(cursorForPosition(pt).position(), QTextCursor::KeepAnchor);
			cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		}
		setTextCursor(cursor);
	}
}
