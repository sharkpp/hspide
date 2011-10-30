#include <QtGui>
#include "editor.h"

//////////////////////////////////////////////////////////////////////

CEditor::CEditor(QWidget *parent)
	: QWidget(parent)
	, m_item(NULL)
{
	m_editor = new CCodeEdit(this);
}

void CEditor::resizeEvent(QResizeEvent * event)
{
	m_editor->resize(event->size());
}

void CEditor::focusInEvent(QFocusEvent * event)
{
	m_editor->setFocus();
}

// シンボル一覧を指定
void CEditor::setSymbols(const QVector<QStringList> & symbols)
{
	m_editor->setSymbols(symbols);
}

// ファイルから読み込み
bool CEditor::load(const QString & filepath)
{
	QFile file(filepath);

	if( !file.open(QFile::ReadOnly | QFile::Text) )
	{
		return false;
	}

	m_filePath = filepath;
	m_editor->setPlainText(file.readAll());

	return true;
}

// ファイルに保存
bool CEditor::save(const QString & filepath)
{
	if( isNoTitle() &&
		filepath.isEmpty() )
	{
		// 保持しているファイル名も新たに指定されたファイル名も無い場合
		// 保存できないのでエラーとする
		return false;
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
bool CEditor::setAssignItem(CProjectItem * item)
{
	m_item = item;
	return true;
}

CProjectItem * CEditor::assignItem()
{
	return m_item;
}

// ファイルパスを取得
const QString & CEditor::filePath() const
{
	return m_filePath;
}

// ファイル名を取得
QString CEditor::fileName() const
{
	return isNoTitle()
			? tr("(no title)")
			: QFileInfo(m_filePath).fileName();
}

// 空ファイルか？
bool CEditor::isNoTitle() const
{
	return m_filePath.isEmpty();
}
