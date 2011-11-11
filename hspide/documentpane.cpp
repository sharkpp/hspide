#include <QtGui>
#include "documentpane.h"

//////////////////////////////////////////////////////////////////////

CDocumentPane::CDocumentPane(QWidget *parent)
	: QWidget(parent)
	, m_item(NULL)
{
	m_editor = new CCodeEdit(this);
}

void CDocumentPane::resizeEvent(QResizeEvent * event)
{
	m_editor->resize(event->size());
}

void CDocumentPane::focusInEvent(QFocusEvent * event)
{
	m_editor->setFocus();
}

// シンボル一覧を指定
void CDocumentPane::setSymbols(const QVector<QStringList> & symbols)
{
	m_editor->setSymbols(symbols);
}

// ファイルから読み込み
bool CDocumentPane::load(const QString & filepath)
{
	QFile file(filepath);

	if( file.open(QFile::ReadOnly | QFile::Text) )
	{
		m_editor->setPlainText(file.readAll());
	}

	m_filePath = filepath;

	return true;
}

// ファイルに保存
bool CDocumentPane::save(const QString & filepath)
{
	if( isUntitled() &&
		filepath.isEmpty() )
	{
		QString filepath
			= QFileDialog::getSaveFileName(this,
				tr("Save File"), m_filePath, tr("Hot Soup Processor Files (*.hsp *.as)"));
		if( filepath.isEmpty() )
		{
			// 保持しているファイル名も新たに指定されたファイル名も無い場合
			// 保存できないのでエラーとする
			return false;
		}
	}

	QFile file(filepath.isEmpty() ? m_filePath : filepath);

	if( !file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text) )
	{
		return false;
	}

	QTextStream out(&file);
	out << m_editor->toPlainText();

	return true;
}

// アイテムと関連付け
bool CDocumentPane::setAssignItem(CWorkSpaceItem * item)
{
	m_item = item;
	return true;
}

CWorkSpaceItem * CDocumentPane::assignItem()
{
	return m_item;
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
