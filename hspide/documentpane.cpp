#include <QtGui>
#include "documentpane.h"

//////////////////////////////////////////////////////////////////////

CDocumentPane::CDocumentPane(QWidget *parent)
	: QWidget(parent)
	, editorWidget(new CCodeEdit(this))
	, m_assignItem(NULL)
{
}

void CDocumentPane::resizeEvent(QResizeEvent * event)
{
	editorWidget->resize(event->size());
}

void CDocumentPane::focusInEvent(QFocusEvent * event)
{
	editorWidget->setFocus();
}

void CDocumentPane::onModificationChanged(bool changed)
{
	emit modificationChanged(isUntitled() | changed);
}

// シンボル一覧を指定
void CDocumentPane::setSymbols(const QVector<QStringList> & symbols)
{
	editorWidget->setSymbols(symbols);
}

// ファイルから読み込み
bool CDocumentPane::load(const QString & filepath, const QString & tmplFilePath)
{
	QFile file(tmplFilePath.isEmpty() ? filepath : tmplFilePath);

	if( !file.open(QFile::ReadOnly | QFile::Text) )
	{
		return false;
	}

	m_filePath = filepath;

	editorWidget->setPlainText(file.readAll());

	// 変更通知シグナルに接続
	connect(editorWidget->document(), SIGNAL(modificationChanged(bool)), this, SLOT(onModificationChanged(bool)));

	return true;
}

// ファイルに保存
bool CDocumentPane::save(const QString & filePath)
{
	QString fileName = filePath;

	// 引数にファイル名が指定されていた場合はそのファイルに保存
	// 指定されていなかった場合は、
	// (無題)で無い場合はそのファイルに保存
	// (無題)の場合はファイル指定ダイアログを表示し指定

	if( fileName.isEmpty() )
	{
		fileName = m_filePath;
		// 無題の場合は適当なファイル名を付ける
		if( isUntitled() )
		{
			QFileInfo fileInfo(fileName);

			fileName
				= fileInfo.dir()
					.absoluteFilePath(tr("untitled") + fileInfo.fileName());
			fileName = QDir::toNativeSeparators(fileName);

			m_filePath = QFileDialog::getSaveFileName(this,
							tr("Save File"), fileName,
							tr("Hot Soup Processor Files (*.hsp *.as)"));
			if( isUntitled() )
			{
				// 保持しているファイル名も新たに指定されたファイル名も無い場合
				// 保存できないのでエラーとする
				return false;
			}
		}
	}

	QFile file(fileName);

	if( !file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text) )
	{
		return false;
	}

	QTextStream out(&file);
	out << editorWidget->toPlainText();

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

// ファイルパスを取得
const QString & CDocumentPane::filePath() const
{
	return m_filePath;
}

// ファイル名を取得
QString CDocumentPane::fileName() const
{
	return isUntitled()
			? tr("(untitled)")
			: QFileInfo(m_filePath).fileName();
}

// 空ファイルか？
bool CDocumentPane::isUntitled() const
{
	return QFileInfo(m_filePath).baseName().isEmpty();
}

// 変更されているか
bool CDocumentPane::isModified() const
{
	return
		isUntitled() ||
		editorWidget->document()->isModified();
}