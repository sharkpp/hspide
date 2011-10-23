#include <QtGui>
#include "editor.h"

//////////////////////////////////////////////////////////////////////

CEditor::CEditor(QWidget *parent)
	: QWidget(parent)
{
	mEditor = new CCodeEdit(this);
}

void CEditor::resizeEvent(QResizeEvent * event)
{
	mEditor->resize(event->size());
}

// ファイルから読み込み
bool CEditor::load(const QString & filepath)
{
	QFile file(filepath);

	if( !file.open(QFile::ReadOnly | QFile::Text) )
	{
		return false;
	}

	mFilePath = filepath;
	mEditor->setPlainText(file.readAll());

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

	QFile file(filepath.isEmpty() ? mFilePath : filepath);

	if( !file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text) )
	{
		return false;
	}

	QTextStream out(&file);
	out << mEditor->toPlainText();

	return true;
}

// ファイルパスを取得
const QString & CEditor::filePath() const
{
	return mFilePath;
}

// ファイル名を取得
QString CEditor::fileName() const
{
	return isNoTitle()
			? tr("(no title)")
			: QFileInfo(mFilePath).fileName();
}

// 空ファイルか？
bool CEditor::isNoTitle() const
{
	return mFilePath.isEmpty();
}
