#include <QtGui>
#include "documentpane.h"
#include "global.h"

//////////////////////////////////////////////////////////////////////

CDocumentPane::CDocumentPane(QWidget *parent)
	: QWidget(parent)
	, m_editorWidget(new CCodeEdit(this))
	, m_highlighter(new CHsp3Highlighter(m_editorWidget->document()))
	, m_assignItem(NULL)
{
	connect(m_editorWidget,             SIGNAL(pressIconArea(int)),        this, SLOT(onPressEditorIconArea(int)));
	connect(m_editorWidget->document(), SIGNAL(modificationChanged(bool)), this, SLOT(onModificationChanged(bool)));

	m_breakPointIcon = m_editorWidget->registLineIcon(QIcon(":/images/icons/small/breakpoint.png"));
	m_cursorIcon     = m_editorWidget->registLineIcon(QIcon(":/images/tango/small/go-next.png"));

	// 設定の変更通史の登録と設定からの初期化処理
	connect(theConf, SIGNAL(updateConfiguration(const Configuration*)),
	        this,  SLOT(updateConfiguration(const Configuration*)));
	updateConfiguration(theConf);

	connect(theBreakPoint, SIGNAL(breakPointChanged(const QUuid&, const QList<QPair<int, bool> >&)),
	        this,  SLOT(onBreakPointChanged(const QUuid&, const QList<QPair<int, bool> >&)));
}

void CDocumentPane::resizeEvent(QResizeEvent* event)
{
	m_editorWidget->resize(event->size());
}

void CDocumentPane::focusInEvent(QFocusEvent* /*event*/)
{
	m_editorWidget->setFocus();
}

void CDocumentPane::onModificationChanged(bool changed)
{
	emit modificationChanged(isUntitled() | changed);
}

void CDocumentPane::onPressEditorIconArea(int lineNo)
{
	if( !m_editorWidget->lineIcon(lineNo, m_breakPointIcon) &&
		!m_uuid.isNull() )
	{
		// ブレークポイントをセット
		theBreakPoint->append(m_uuid, lineNo + 1); // 1オリジン
	}
	else
	{
		// ブレークポイントをリセット
		theBreakPoint->remove(m_uuid, lineNo + 1); // 1オリジン
	}

	emit updateBreakpoint();
}

void CDocumentPane::updateConfiguration(const Configuration* conf)
{
	QFont editorFont;
	Configuration::ColorMetricsInfoType metrics;

	// フォントを変更
	editorFont.setFamily(conf->editorFontName());
	editorFont.setPixelSize(conf->editorFontSize());
	editorFont.setFixedPitch(true);
	m_editorWidget->setFont(editorFont);

	// 行番号フォントを変更
	editorFont.setFamily(conf->editorLineNumberFontName());
	editorFont.setPixelSize(conf->editorLineNumberFontSize());
	editorFont.setFixedPitch(true);
	m_editorWidget->setLineNumberFont(editorFont);

	m_editorWidget->setLineNumberVisible(conf->editorLineNumberVisibled());
	m_editorWidget->setTabStopCharWidth(conf->editorTabWidth());

	// 色を変更
	metrics = conf->colorMetrics(Configuration::LineNumberMetrics);
	m_editorWidget->setLineNumberTextColor(metrics.foregroundColor);
	m_editorWidget->setLineNumberBackgroundColor(metrics.backgroundColor);

	QPalette palette = m_editorWidget->palette();
	palette.setColor(QPalette::Window, metrics.backgroundColor);
	metrics = conf->colorMetrics(Configuration::TextMetrics);
	palette.setColor(QPalette::Text, metrics.foregroundColor);
	palette.setColor(QPalette::Base, metrics.backgroundColor);
	m_editorWidget->setPalette(palette);

	// キーワードの色を変更
	QVector<QPair<Configuration::ColorMetricsEnum, Hsp3Lexer::Type> > keywordFormat;
	keywordFormat	<< qMakePair(Configuration::FunctionMetrics,     Hsp3Lexer::TypeFunction)
					<< qMakePair(Configuration::SubroutineMetrics,   Hsp3Lexer::TypeSubroutine)
					<< qMakePair(Configuration::PreprocessorMetrics, Hsp3Lexer::TypePreprocesser)
					<< qMakePair(Configuration::MacroMetrics,        Hsp3Lexer::TypeMacro)
					<< qMakePair(Configuration::LabelMetrics,        Hsp3Lexer::TypeLabel)
					<< qMakePair(Configuration::CommentMetrics,      Hsp3Lexer::TypeComment)
					<< qMakePair(Configuration::StringMetrics,       Hsp3Lexer::TypeString)
					<< qMakePair(Configuration::NumberMetrics,       Hsp3Lexer::TypeNumber)
					;
	for(int i = 0; i < keywordFormat.size(); i++)
	{
		QTextCharFormat format;
		Hsp3Lexer::Type type = keywordFormat[i].second;
		metrics = conf->colorMetrics(keywordFormat[i].first);
		format.setForeground(metrics.foregroundColor);
		format.setBackground(metrics.backgroundColor);
		format.setFontWeight(metrics.useBoldFont ? QFont::Bold : QFont::Normal);
		m_highlighter->setFormat(type, format);
	}

	// キーワードハイライトを再描画
	m_highlighter->rehighlight();
}

void CDocumentPane::onBreakPointChanged(const QUuid& uuid, const QList<QPair<int, bool> >& modifiedLineNumberes)
{
	if( m_uuid != uuid ) {
		return;
	}

	for(QList<QPair<int, bool> >::const_iterator
			ite = modifiedLineNumberes.begin(),
			last= modifiedLineNumberes.end();
		ite != last; ++ite)
	{
		if( ite->second ) {
			m_editorWidget->setLineIcon(ite->first - 1, m_breakPointIcon);
		} else {
			m_editorWidget->clearLineIcon(ite->first - 1, m_breakPointIcon);
		}
	}
}

// シンボル一覧を指定
void CDocumentPane::setSymbols(const QVector<QStringList>& symbols)
{
	m_editorWidget->setSymbols(symbols);
	m_highlighter->setSymbols(symbols);
}

// ファイルから読み込み
bool CDocumentPane::load(const QString& filepath, const QString& tmplFilePath)
{
	QFile file(tmplFilePath.isEmpty() ? filepath : tmplFilePath);

	if( !file.open(QFile::ReadOnly | QFile::Text) )
	{
		return false;
	}

	if( !m_uuid.isNull() )
	{
		theFile->rename(m_uuid, filepath);
	}
	else
	{
		m_uuid = theFile->assign(filepath);
	}

	m_editorWidget->setPlainText(file.readAll());

	// テンプレートから追加した時は初期時に変更ありにする
	if( !tmplFilePath.isEmpty() ) {
		m_editorWidget->document()->setModified();
	}

	// 変更通知シグナルに接続
	connect(m_editorWidget->document(), SIGNAL(modificationChanged(bool)), this, SLOT(onModificationChanged(bool)));

	return true;
}

// ファイルに保存
bool CDocumentPane::save(const QString& filePath, bool saveAs, bool temporary)
{
	QString fileName = filePath;
	QString lastFilePath = theFile->path(m_uuid);

	// 引数にファイル名が指定されていた場合はそのファイルに保存
	// 指定されていなかった場合は、
	// (無題)で無い場合はそのファイルに保存
	// (無題)の場合はファイル指定ダイアログを表示し指定

	if( fileName.isEmpty() || saveAs )
	{
		if( fileName.isEmpty() || !saveAs ) {
			fileName = lastFilePath;
		}
		// 無題の場合は適当なファイル名を付ける
		if( isUntitled() || saveAs )
		{
			QFileInfo fileInfo(fileName);

			fileName
				= fileInfo.dir()
					.absoluteFilePath(
							isUntitled() || !saveAs
								? tr("untitled") + (fileInfo.suffix().isEmpty() ? "" : "." + fileInfo.suffix())
								: fileInfo.fileName()
						);
			fileName = QDir::toNativeSeparators(fileName);

			fileName = QFileDialog::getSaveFileName(this,
							tr("Save File"), fileName,
							tr("Hot Soup Processor Files (*.hsp *.as)"));
			if( QFileInfo(fileName).baseName().isEmpty() )
			{
				// 保持しているファイル名も新たに指定されたファイル名も無い場合
				// 保存できないのでエラーとする
				return false;
			}

			fileName = QDir::toNativeSeparators(fileName);
		}
	}

	// 変更が加えられているか？
	if( lastFilePath == fileName &&
		!m_editorWidget->document()->isModified() )
	{
		// 変更されていなければファイルへは保存せずに戻る
		return true;
	}

	QFile file(fileName);

	if( !file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text) )
	{
		return false;
	}

	QTextStream out(&file);
	out << m_editorWidget->toPlainText();

	if( !temporary )
	{
		m_editorWidget->document()->setModified(false);
		theFile->rename(m_uuid, fileName);
	}

	return true;
}

// 指定行に移動
bool CDocumentPane::jump(int lineNo)
{
	QTextCursor cursor = m_editorWidget->textCursor();
	cursor.setPosition(0, QTextCursor::MoveAnchor);
	cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineNo - 1);
	m_editorWidget->setTextCursor(cursor);
	setFocus();
	return true;
}

// アイテムと関連付け
bool CDocumentPane::setAssignItem(CWorkSpaceItem* item)
{
	if( m_assignItem ) {
		m_assignItem->setAssignDocument(NULL);
	}
	m_assignItem = item;
	m_assignItem->setAssignDocument(this);
	m_uuid = m_assignItem->uuid();
	return true;
}

CWorkSpaceItem* CDocumentPane::assignItem()
{
	return m_assignItem;
}

// uuidを取得
const QUuid& CDocumentPane::uuid() const
{
	return m_uuid;
}

// ファイルパスを取得
QString CDocumentPane::path() const
{
	return theFile->path(m_uuid);
}

// タイトルを取得
QString CDocumentPane::title() const
{
	return theFile->fileName(m_uuid) + (m_editorWidget->document()->isModified() ? "*" : "");
}

// 無題ファイル(ディスクに保存していないファイル)か？
bool CDocumentPane::isUntitled() const
{
	return theFile->isUntitled(m_uuid);
}

// 変更されているか
bool CDocumentPane::isModified() const
{
	return
		m_editorWidget->document() &&
		m_editorWidget->document()->isModified();
}

void CDocumentPane::markCursorLine(const QUuid& uuid, int lineNo)
{
	if( m_uuid != uuid ) {
		return;
	}

	m_editorWidget->clearLineIcon(-1, m_cursorIcon);
	m_editorWidget->setLineIcon(lineNo - 1, m_cursorIcon);
}

void CDocumentPane::unmarkCursorLine()
{
	m_editorWidget->clearLineIcon(-1, m_cursorIcon);
}
