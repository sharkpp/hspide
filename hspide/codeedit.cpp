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

	virtual void paintEvent(QPaintEvent* event)
	{
		static_cast<CCodeEdit*>(parentWidget())->paintLineNumEvent(event);
	}

	virtual void mousePressEvent(QMouseEvent* event)
	{
		static_cast<CCodeEdit*>(parentWidget())->mousePressLineNumEvent(event);
	}

	virtual void mouseReleaseEvent(QMouseEvent* event)
	{
		static_cast<CCodeEdit*>(parentWidget())->mouseReleaseLineNumEvent(event);
	}

	virtual void mouseMoveEvent(QMouseEvent* event)
	{
		static_cast<CCodeEdit*>(parentWidget())->mouseMoveLineNumEvent(event);
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
	m_completer        = new QCompleter(this);

	m_completer->setWidget(this);
	m_completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
	m_completer->setCaseSensitivity(Qt::CaseInsensitive);
	m_completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	m_completer->setWrapAround(false);

	connect(this, SIGNAL(blockCountChanged(int)),   this, SLOT(updateLineNumberWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumber(QRect,int)));
	connect(m_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));

	updateLineNumberWidth(0);

	// タブストップを設定
	setTabStopCharWidth(m_tabStopCharWidth);
}

// シンボル一覧を指定
void CCodeEdit::setSymbols(const QVector<QStringList>& symbols)
{
//	QStandardItemModel* model = new QStandardItemModel(this);

	QStringList keywords;

	foreach(const QStringList &symbol, symbols)
	{
		keywords << symbol[0];
	}
	keywords.sort();

qDebug()<<keywords;
	m_completer->setModel(new QStringListModel(keywords, m_completer));
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

void CCodeEdit::setLineNumberFont(const QFont& font)
{
	m_lineNumberWidget->setFont(font);
	updateLineNumberWidth(0);
}

const QFont& CCodeEdit::lineNumberFont() const
{
	return m_lineNumberWidget->font();
}

void CCodeEdit::setLineIconBackgroundColorRole(const QPalette::ColorRole& role)
{
	m_lineIconBackgroundColorRole = role;
}

void CCodeEdit::setLineNumberBackgroundColorRole(const QPalette::ColorRole& role)
{
	m_lineNumberBackgroundColorRole = role;
}

void CCodeEdit::setLineNumberTextColorRole(const QPalette::ColorRole& role)
{
	m_lineNumberTextColorRole = role;
}

void CCodeEdit::setLineIconBackgroundColor(const QColor& color)
{
	m_lineIconBackgroundColor = color;
	m_lineIconBackgroundColorRole = QPalette::NoRole;
}

void CCodeEdit::setLineNumberBackgroundColor(const QColor& color)
{
	m_lineNumberBackgroundColor = color;
	m_lineNumberBackgroundColorRole = QPalette::NoRole;
}

void CCodeEdit::setLineNumberTextColor(const QColor& color)
{
	m_lineNumberTextColor = color;
	m_lineNumberTextColorRole = QPalette::NoRole;
}

void CCodeEdit::setLineNumberVisible(bool visible)
{
	m_visibleLineNumber = visible;
}

int CCodeEdit::registLineIcon(const QIcon& icon)
{
	int no = !m_iconMap.isEmpty() ? m_iconMap.keys().back() + 1 : 0;
    m_iconMap[no] = new QIcon(icon);
	return no;
}

void CCodeEdit::unregistLineIcon(int iconNo)
{
	m_iconMap.remove(iconNo);
}

bool CCodeEdit::lineIcon(int lineNo, int iconNo) const
{
    QMap<int, QSet<int> >::const_iterator
		ite = m_lineIconMap.find(lineNo);
	if( m_lineIconMap.end() != ite )
	{
        QSet<int>::const_iterator
			ite2 = ite.value().find(iconNo);
		if( ite.value().end() != ite2 )
		{
			return true;
		}
	}
	return false;
}

void CCodeEdit::setLineIcon(int lineNo, int iconNo)
{
	m_lineIconMap[lineNo].insert(iconNo);
	m_lineNumberWidget->update(0, contentsRect().y(), m_lineNumberWidget->width(), contentsRect().height());
}

void CCodeEdit::clearLineIcon()
{
	m_lineIconMap.clear();
	m_lineNumberWidget->update(0, contentsRect().y(), m_lineNumberWidget->width(), contentsRect().height());
}

void CCodeEdit::clearLineIcon(int lineNo)
{
	QMap<int, QSet<int> >::iterator
		ite = m_lineIconMap.find(lineNo);
	if( m_lineIconMap.end() != ite )
	{
		m_lineIconMap.erase(ite);
	}
	m_lineNumberWidget->update(0, contentsRect().y(), m_lineNumberWidget->width(), contentsRect().height());
}

void CCodeEdit::clearLineIcon(int lineNo, int iconNo)
{
	for(QMap<int, QSet<int> >::iterator
			ite = m_lineIconMap.begin();
		ite != m_lineIconMap.end(); )
	{
		if( lineNo < 0 ||
			lineNo == ite.key() )
		{
			QSet<int>::iterator
				ite2 = ite.value().find(iconNo);
			if( ite.value().end() != ite2 )
			{
				ite.value().erase(ite2);
				if( ite.value().isEmpty() ) {
					m_lineIconMap.erase(ite++);
					continue;
				}
			}
		}
		++ite;
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

void CCodeEdit::updateLineNumberWidth(int /*newBlockCount*/)
{
	setViewportMargins(lineNumberWidth(), 0, 0, 0);
}

void CCodeEdit::updateLineNumber(const QRect& rect ,int /*dy*/)
{
	m_lineNumberWidget->update(0, rect.y(), m_lineNumberWidget->width(), rect.height());
}

void CCodeEdit::insertCompletion(const QString& completion)
{
	if( m_completer->widget() != this ) {
		return;
	}

	QTextCursor cursor = textCursor();
	QString completionPrefix = m_completer->completionPrefix();
	int len = completionPrefix.length();
	int extra = completion.length() - len;

	if( completionPrefix == completion.left(len) )
	{
		cursor.movePosition(QTextCursor::EndOfWord);
		cursor.insertText(completion.right(extra));
	}
	else
	{
		cursor.movePosition(QTextCursor::StartOfWord);
		cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		cursor.insertText(completion);
	}
	setTextCursor(cursor);
}

void CCodeEdit::del()
{
	QTextCursor cursor = textCursor();
	cursor.deleteChar();
}

void CCodeEdit::keyPressEvent(QKeyEvent* event)
{
	QString text = event->text();

	bool shiftModifier = 0 != (event->modifiers() & Qt::ShiftModifier);
	bool ctrlModifier  = 0 != (event->modifiers() & Qt::ControlModifier);
	bool pressTab      = Qt::Key_Tab   == event->key();
	bool pressSpace    = Qt::Key_Space == event->key();
	bool complitePopup = m_completer && m_completer->popup()->isVisible();

	// 入力補間中
	if( complitePopup )
	{
		switch( event->key() )
		{
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Escape:
		case Qt::Key_Tab:
		case Qt::Key_Backtab:
			event->ignore();
			return;
		default:
			break;
		}
	}

	// SHIFT+ENTERをキャンセルして普通の入力にする
	if( shiftModifier && Qt::Key_Return == event->key() )
	{
		QTextCursor cursor = textCursor();
		cursor.insertText("\n");
		event->ignore();
		return;
	}

	// CTRL+SPACE → 入力補間開始 or 入力補間中
	if( complitePopup ||
		(!text.isEmpty() && ctrlModifier && !shiftModifier && pressSpace) )
	{
		if( complitePopup )
		{
			QPlainTextEdit::keyPressEvent(event);
		}

		QTextCursor cursor = textCursor();
		cursor.select(QTextCursor::WordUnderCursor);
		QString completionPrefix = cursor.selectedText();
		if( completionPrefix != m_completer->completionPrefix())
		{
			m_completer->setCompletionPrefix(completionPrefix);
			m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
		}

		if( complitePopup && completionPrefix.isEmpty() )
		{
			m_completer->popup()->hide();
		}
		else
		{
			// ポップアップを表示
			QRect rc = cursorRect();
			rc.moveLeft(lineNumberWidth());
			rc.setWidth(m_completer->popup()->sizeHintForColumn(0)
						+ m_completer->popup()->verticalScrollBar()->sizeHint().width());
			m_completer->complete(rc);
		}

		return;
	}

	// SPACE or SHIFT+SPACE or TAB or SHIFT+TAB
	if( !text.isEmpty() && !ctrlModifier && (pressTab || pressSpace) ) 
	{
		QChar c = text[0];
		QTextCursor cursor = textCursor();
		// 現在位置を取得
		int startSel = cursor.selectionStart();
		int endSel   = cursor.selectionEnd();
		// 選択行数を取得しインデントするかどうかを判断
		cursor.setPosition(startSel, QTextCursor::KeepAnchor);
		int startLine = cursor.blockNumber();
		cursor.setPosition(endSel, QTextCursor::KeepAnchor);
		int endLine   = cursor.blockNumber();

QTime time; time.start();
		if( startLine < endLine ) 
		{
			// 選択し直す
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
				if( !shiftModifier )
				{
					// TAB or スペースでインデント
					for(int i = 0, num = lines.size(); i < num; i++)
					{
						// 空行ではなければインデント処理
						if( !lines[i].isEmpty() )
						{
							lines[i].insert(0, c);
							endSel++;
						}
					}
				}
				else
				{
					if( ' ' == c ) {
						// スペースで逆インデント
						for(int i = 0, num = lines.size(); i < num; i++)
						{
							if( c == lines[i][0] ) {
								lines[i].remove(0, 1);
								endSel--;
							}
						}
					} else {
						// TAB or スペースで逆インデント
						for(int i = 0, num = lines.size(); i < num; i++)
						{
							if( c == lines[i][0] ) {
								lines[i].remove(0, 1);
								endSel--;
							} else {
								// スペースの場合、最大TAB文字の幅分逆インデント
								int j = 0;
								for(; j < m_tabStopCharWidth &&
									' ' == lines[i][j]; j++) {
								}
								lines[i].remove(0, j);
								endSel -= j;
							}
						}
					}
				}
				if( skipLastLine ) {
					lines.append("");
				}
			}
			QString text = lines.join(QString(QChar::ParagraphSeparator));
qDebug() << __LINE__ << time.elapsed();
time.start();

			cursor.beginEditBlock();
			cursor.insertText(text);
			cursor.endEditBlock();
qDebug() << __LINE__ << time.elapsed();

			cursor.setPosition(startSel);
			cursor.movePosition(QTextCursor::StartOfLine);
			cursor.setPosition(endSel - 1, QTextCursor::KeepAnchor);
			cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);

			setTextCursor(cursor);

			return;
		}
	}

	QPlainTextEdit::keyPressEvent(event);
}

void CCodeEdit::resizeEvent(QResizeEvent* event)
{
	QPlainTextEdit::resizeEvent(event);

	const QRect& rc = contentsRect();
	m_lineNumberWidget->setGeometry(rc.left(), rc.top(), lineNumberWidth(), rc.height());
}

void CCodeEdit::paintLineNumEvent(QPaintEvent* event)
{
	QPainter painter(m_lineNumberWidget);

	// 行番号エリアをベタ塗り
	const QRect& rc = event->rect();
	painter.fillRect(rc, lineNumberBackgroundColor());
	painter.fillRect(QRect(	rc.left(), rc.top(),
							rc.left() + m_lineIconSize.width(),
							rc.height()), lineIconBackgroundColor());

	if( m_visibleLineNumber )
	{
		// 行番号との区切りを描画
		const QRect& rcWnd = contentsRect();
		QPen pen;
		pen.setColor(lineNumberTextColor());
		pen.setStyle(Qt::DotLine);
		painter.setPen(pen);
		// ※クリックしたりするとごみが描画されるので毎回、上から下までを描画する
		painter.fillRect(QRect(	rc.right(), rcWnd.top(), rc.right(), rcWnd.height()),
						 lineIconBackgroundColor());
		painter.drawLine(rc.right(), rcWnd.top(), rc.right(), rcWnd.bottom());
	}

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
		QRectF rect = blockBoundingGeometry(block).translated(contentOffset());
		y = (int)rect.top();
		// 行番号を描画
		if( m_visibleLineNumber )
		{
			painter.drawText(0, y,
							 lineNumWidth - lineNumSpace, lineHeight,
							 Qt::AlignRight, QString::number(lineNo + 1));
		}
		// アイコンを描画
		QMap<int, QSet<int> >::iterator
			ite = m_lineIconMap.find(lineNo);
		if( m_lineIconMap.end() != ite )
		{
			for(QSet<int>::iterator
					ite2 = ite.value().begin(),
					last2= ite.value().end();
				ite2 != last2; ++ite2)
			{
                QIcon* icon = m_iconMap[*ite2];
                QPixmap pixmap = icon->pixmap(QSize(m_lineIconSize.width() - 2, m_lineIconSize.height() - 2), QIcon::Normal);
				painter.drawPixmap(QPoint(1, y + int(rect.height() - m_lineIconSize.height()) + 1), pixmap);
			}
		}
	}
}

void CCodeEdit::mousePressLineNumEvent(QMouseEvent* event)
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

void CCodeEdit::mouseReleaseLineNumEvent(QMouseEvent* event)
{
}

void CCodeEdit::mouseMoveLineNumEvent(QMouseEvent* event)
{
	QPoint pt(0, event->y());

	if( m_lineIconSize.width() < event->x() &&
		0 != (event->buttons()& Qt::LeftButton) )
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
