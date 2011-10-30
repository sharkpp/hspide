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

// �V���{���ꗗ���w��
void CEditor::setSymbols(const QVector<QStringList> & symbols)
{
	m_editor->setSymbols(symbols);
}

// �t�@�C������ǂݍ���
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

	QFile file(filepath.isEmpty() ? m_filePath : filepath);

	if( !file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text) )
	{
		return false;
	}

	QTextStream out(&file);
	out << m_editor->toPlainText();

	return true;
}

// �A�C�e���Ɗ֘A�t��
bool CEditor::setAssignItem(CProjectItem * item)
{
	m_item = item;
	return true;
}

CProjectItem * CEditor::assignItem()
{
	return m_item;
}

// �t�@�C���p�X���擾
const QString & CEditor::filePath() const
{
	return m_filePath;
}

// �t�@�C�������擾
QString CEditor::fileName() const
{
	return isNoTitle()
			? tr("(no title)")
			: QFileInfo(m_filePath).fileName();
}

// ��t�@�C�����H
bool CEditor::isNoTitle() const
{
	return m_filePath.isEmpty();
}
