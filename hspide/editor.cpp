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

// �t�@�C������ǂݍ���
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

// �t�@�C���ɕۑ�
bool CEditor::save(const QString & filepath)
{
	if( isNoTitle() &&
		filepath.isEmpty() )
	{
		// �ێ����Ă���t�@�C�������V���Ɏw�肳�ꂽ�t�@�C�����������ꍇ
		// �ۑ��ł��Ȃ��̂ŃG���[�Ƃ���
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

// �t�@�C���p�X���擾
const QString & CEditor::filePath() const
{
	return mFilePath;
}

// �t�@�C�������擾
QString CEditor::fileName() const
{
	return isNoTitle()
			? tr("(no title)")
			: QFileInfo(mFilePath).fileName();
}

// ��t�@�C�����H
bool CEditor::isNoTitle() const
{
	return mFilePath.isEmpty();
}
