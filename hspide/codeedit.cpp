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

	QHash<QString, QTextCharFormat> m_keywords;

	QTextCharFormat m_format[Hsp3Lexer::TypeNum];

public:

	CHighlighter(QTextDocument *parent)
		: QSyntaxHighlighter(parent)
	{
		// ���x��
		QTextCharFormat &labelFormat = m_format[Hsp3Lexer::TypeLabel];
		labelFormat.setForeground(Qt::darkYellow);
	//	labelFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);

		// �R�����g
		QTextCharFormat &commentFormat = m_format[Hsp3Lexer::TypeComment];
		commentFormat.setForeground(Qt::darkGreen);
	//	commentFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);

		// ������
		QTextCharFormat &stringFormat = m_format[Hsp3Lexer::TypeString];
		stringFormat.setForeground(Qt::darkRed);
	//	stringFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
	}

	virtual ~CHighlighter()
	{
	}

	// �V���{���ꗗ���w��
	void setSymbols(const QVector<QStringList> &symbols)
	{
		m_keywords.clear();

		// �֐�
		QTextCharFormat functionFormat;
		functionFormat.setForeground(Qt::blue);

		// �v���v���Z�b�T
		QTextCharFormat preprocesserFormat;
		preprocesserFormat.setForeground(Qt::blue);

		// ��`�ς݃}�N��
		QTextCharFormat macroFormat;
		macroFormat.setForeground(QBrush(QColor(0,128,255)));

		foreach(const QStringList &symbol, symbols)
		{
			const QString &category1 = symbol[1];
			const QString &category2 = symbol[2];
			if( !category2.compare("macro") ) {
				// ��`�ς݃}�N��
				m_keywords[symbol[0]] = macroFormat;
			} else if( !category2.compare("func") ) {
				if( !category1.compare("pre") ) {
					// �v���v���Z�b�T
					m_keywords[symbol[0]] = preprocesserFormat;
				} else {
					// �֐�/����
					m_keywords[symbol[0]] = functionFormat;
				}
			}
		}
	}

private:

	virtual void highlightBlock(const QString& text)
	{
		Hsp3Lexer lexer;

		lexer.reset(QString(text).append(QChar::LineSeparator), previousBlockState());

		while( lexer.scan() )
		{
			if( Hsp3Lexer::TypeNormal == lexer.type() )
			{
				QString token = lexer.text().toLower();
				if( m_keywords.contains(token) )
				{
					setFormat(lexer.start(), lexer.length(), m_keywords[token]);
				}
			}
			else
			{
				setFormat(lexer.start(), lexer.length(), m_format[lexer.type()]);
			}
		}

		setCurrentBlockState( lexer.state() );

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

	// �t�H���g��ύX
	// ����芸�����̃e�X�g
	QFont font_ = font();
	font_.setFamily("MS Gothic");
	font_.setFixedPitch(true);
//	font_.setPixelSize(16);
	setFont(font_);

	// �^�u�X�g�b�v��ݒ�
	setTabStopCharWidth(m_tabStopCharWidth);
}

// �V���{���ꗗ���w��
void CCodeEdit::setSymbols(const QVector<QStringList> & symbols)
{
	static_cast<CHighlighter*>(m_highlighter)->setSymbols(symbols);

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
// �v���p�e�B
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
// �C�x���g
//////////////////////////////////////////////////////////////////////

void CCodeEdit::updateLineNumberWidth(int /*newBlockCount*/)
{
	setViewportMargins(lineNumberWidth(), 0, 0, 0);
}

void CCodeEdit::updateLineNumber(const QRect & rect ,int dy)
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

	// ���͕�Ԓ�
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

	// CTRL+SPACE �� ���͕�ԊJ�n or ���͕�Ԓ�
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
			// �|�b�v�A�b�v��\��
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
		// ���݈ʒu���擾
		int startSel = cursor.selectionStart();
		int endSel   = cursor.selectionEnd();
		// �I���s�����擾���C���f���g���邩�ǂ����𔻒f
		cursor.setPosition(startSel, QTextCursor::KeepAnchor);
		int startLine = cursor.blockNumber();
		cursor.setPosition(endSel, QTextCursor::KeepAnchor);
		int endLine   = cursor.blockNumber();

QTime time; time.start();
		if( startLine < endLine ) 
		{
			// �I��������
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
					// TAB or �X�y�[�X�ŃC���f���g
					for(int i = 0, num = lines.size(); i < num; i++)
					{
						// ��s�ł͂Ȃ���΃C���f���g����
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
						// �X�y�[�X�ŋt�C���f���g
						for(int i = 0, num = lines.size(); i < num; i++)
						{
							if( c == lines[i][0] ) {
								lines[i].remove(0, 1);
								endSel--;
							}
						}
					} else {
						// TAB or �X�y�[�X�ŋt�C���f���g
						for(int i = 0, num = lines.size(); i < num; i++)
						{
							if( c == lines[i][0] ) {
								lines[i].remove(0, 1);
								endSel--;
							} else {
								// �X�y�[�X�̏ꍇ�A�ő�TAB�����̕����t�C���f���g
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

	// �s�ԍ��G���A���x�^�h��
	const QRect & rc = event->rect();
	painter.fillRect(rc, lineNumberBackgroundColor());
	painter.fillRect(QRect(	rc.left(), rc.top(),
							rc.left() + m_lineIconSize.width(),
							rc.height()), lineIconBackgroundColor());

	// �s�ԍ��Ƃ̋�؂��`��
	const QRect & rcWnd = contentsRect();
	QPen pen;
	pen.setColor(lineNumberTextColor());
	pen.setStyle(Qt::DotLine);
	painter.setPen(pen);
	// ���N���b�N�����肷��Ƃ��݂��`�悳���̂Ŗ���A�ォ�牺�܂ł�`�悷��
	painter.fillRect(QRect(	rc.right(), rcWnd.top(), rc.right(), rcWnd.height()),
					 lineIconBackgroundColor());
	painter.drawLine(rc.right(), rcWnd.top(), rc.right(), rcWnd.bottom());

	// �s�ԍ���`��
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
		// �s�ԍ���`��
		QRectF rect = blockBoundingGeometry(block).translated(contentOffset());
		y = (int)rect.top();
		painter.drawText(0, y,
		                 lineNumWidth - lineNumSpace, lineHeight,
		                 Qt::AlignRight, QString::number(lineNo + 1));
		// �A�C�R����`��
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
		// �A�C�R���������N���b�N���ꂽ
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
			// �I���J�n�ʒu���O��I��
			cursor.setPosition(cursor.selectionEnd());
			cursor.movePosition(QTextCursor::EndOfLine);
			cursor.setPosition(cursorForPosition(pt).position(), QTextCursor::KeepAnchor);
			cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
		}
		else
		{
			// �I���J�n�ʒu������I��
			cursor.setPosition(cursor.selectionStart());
			cursor.movePosition(QTextCursor::StartOfLine);
			cursor.setPosition(cursorForPosition(pt).position(), QTextCursor::KeepAnchor);
			cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		}
		setTextCursor(cursor);
	}
}
