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

	// 設定の変更通史の登録と設定からの初期化処理
	connect(&theConf, SIGNAL(updateConfiguration(const Configuration&)),
	        this,  SLOT(updateConfiguration(const Configuration&)));
	updateConfiguration(theConf);
}

void CDocumentPane::resizeEvent(QResizeEvent * event)
{
	m_editorWidget->resize(event->size());
}

void CDocumentPane::focusInEvent(QFocusEvent * event)
{
	m_editorWidget->setFocus();
}

void CDocumentPane::onModificationChanged(bool changed)
{
	emit modificationChanged(isUntitled() | changed);
}

void CDocumentPane::onPressEditorIconArea(int lineNo)
{
	if( m_editorWidget->lineIcon(lineNo).isNull() )
	{
		m_editorWidget->setLineIcon(lineNo, QIcon(":/images/icons/small/media-record-blue.png"));
		// ブレークポイントをセット
		if( m_assignItem )
		{
			m_assignItem->setBreakPoint(lineNo + 1); // 1オリジン
		}
	}
	else
	{
		m_editorWidget->clearLineIcon(lineNo);
		// ブレークポイントをリセット
		if( m_assignItem )
		{
			m_assignItem->clearBreakPoint(lineNo + 1); // 1オリジン
		}
	}

	emit updateBreakpoint();
}

void CDocumentPane::updateConfiguration(const Configuration& info)
{
	QFont editorFont;
	Configuration::ColorMetricsInfoType metrics;

	// フォントを変更
	editorFont.setFamily(info.editorFontName());
	editorFont.setPixelSize(info.editorFontSize());
	editorFont.setFixedPitch(true);
	m_editorWidget->setFont(editorFont);

	// 行番号フォントを変更
	editorFont.setFamily(info.editorLineNumberFontName());
	editorFont.setPixelSize(info.editorLineNumberFontSize());
	editorFont.setFixedPitch(true);
	m_editorWidget->setLineNumberFont(editorFont);

	m_editorWidget->setLineNumberVisible(info.editorLineNumberVisibled());
	m_editorWidget->setTabStopCharWidth(info.editorTabWidth());

	// 色を変更
	metrics = info.colorMetrics(Configuration::LineNumberMetrics);
	m_editorWidget->setLineNumberTextColor(metrics.foregroundColor);
	m_editorWidget->setLineNumberBackgroundColor(metrics.backgroundColor);

	QPalette palette = m_editorWidget->palette();
	palette.setColor(QPalette::Window, metrics.backgroundColor);
	metrics = info.colorMetrics(Configuration::TextMetrics);
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
					;
	for(int i = 0; i < keywordFormat.size(); i++)
	{
		QTextCharFormat format;
		Hsp3Lexer::Type type = keywordFormat[i].second;
		metrics = info.colorMetrics(keywordFormat[i].first);
		format.setForeground(metrics.foregroundColor);
		format.setBackground(metrics.backgroundColor);
		format.setFontWeight(metrics.useBoldFont ? QFont::Bold : QFont::Normal);
		m_highlighter->setFormat(type, format);
	}

	// キーワードハイライトを再描画
	m_highlighter->rehighlight();
}

// シンボル一覧を指定
void CDocumentPane::setSymbols(const QVector<QStringList> & symbols)
{
	m_editorWidget->setSymbols(symbols);
	m_highlighter->setSymbols(symbols);
}

// ファイルから読み込み
bool CDocumentPane::load(const QString & filepath, const QString & tmplFilePath)
{
	QFile file(tmplFilePath.isEmpty() ? filepath : tmplFilePath);

	if( !file.open(QFile::ReadOnly | QFile::Text) )
	{
		return false;
	}

	m_uuid = theFile.assign(filepath);

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
bool CDocumentPane::save(const QString & filePath, bool saveAs)
{
	QString fileName = filePath;
	QString lastFilePath = theFile.path(m_uuid);

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
								? tr("untitled") + fileInfo.suffix()
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

	return true;
}

// 行に移動
bool CDocumentPane::jump(int lineNo)
{
	QTextCursor cursor = m_editorWidget->textCursor();
	cursor.setPosition(0, QTextCursor::MoveAnchor);
	cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineNo - 1);
	cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
	m_editorWidget->setTextCursor(cursor);
	return true;
}

// アイテムと関連付け
bool CDocumentPane::setAssignItem(CWorkSpaceItem * item)
{
	if( m_assignItem ) {
		m_assignItem->setAssignDocument(NULL);
	}
	m_assignItem = item;
	m_assignItem->setAssignDocument(this);
	return true;
}

CWorkSpaceItem * CDocumentPane::assignItem()
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
	return theFile.path(m_uuid);
}

// タイトルを取得
QString CDocumentPane::title() const
{
	return theFile.fileName(m_uuid) + (m_editorWidget->document()->isModified() ? "*" : "");
}

// 無題ファイル(ディスクに保存していないファイル)か？
bool CDocumentPane::isUntitled() const
{
	return theFile.isUntitled(m_uuid);
}

// 変更されているか
bool CDocumentPane::isModified() const
{
	return
		m_editorWidget->document() &&
		m_editorWidget->document()->isModified();
}