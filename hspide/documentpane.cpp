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

// �V���{���ꗗ���w��
void CDocumentPane::setSymbols(const QVector<QStringList> & symbols)
{
	m_editor->setSymbols(symbols);
}

// �t�@�C������ǂݍ���
bool CDocumentPane::load(const QString & filepath, const QString & tmplFilePath)
{
	QFile file(tmplFilePath.isEmpty() ? filepath : tmplFilePath);

	if( file.open(QFile::ReadOnly | QFile::Text) )
	{
		m_editor->setPlainText(file.readAll());
	}

	m_filePath = filepath;

	return true;
}

// �t�@�C���ɕۑ�
bool CDocumentPane::save(const QString & filePath)
{
	if( isUntitled() &&
		filePath.isEmpty() )
	{
		// ����̏ꍇ�͓K���ȃt�@�C������t����
		QString fileName = m_filePath;
		if( isUntitled() )
		{
			QFileInfo fileInfo(fileName);
			fileName
				= fileInfo.dir()
					.absoluteFilePath("untitled" + fileInfo.fileName());
			fileName = QDir::toNativeSeparators(fileName);
		}
		m_filePath = QFileDialog::getSaveFileName(this,
						tr("Save File"), fileName,
						tr("Hot Soup Processor Files (*.hsp *.as)"));
		if( isUntitled() )
		{
			// �ێ����Ă���t�@�C�������V���Ɏw�肳�ꂽ�t�@�C�����������ꍇ
			// �ۑ��ł��Ȃ��̂ŃG���[�Ƃ���
			return false;
		}
	}

	QFile file(m_filePath);

	if( !file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text) )
	{
		return false;
	}

	QTextStream out(&file);
	out << m_editor->toPlainText();

	return true;
}

// �A�C�e���Ɗ֘A�t��
bool CDocumentPane::setAssignItem(CWorkSpaceItem * item)
{
	m_item = item;
	return true;
}

CWorkSpaceItem * CDocumentPane::assignItem()
{
	return m_item;
}

// �t�@�C���p�X���擾
const QString & CDocumentPane::filePath() const
{
	return m_filePath;
}

// �t�@�C�������擾
QString CDocumentPane::fileName() const
{
	return isUntitled()
			? tr("(untitled)")
			: QFileInfo(m_filePath).fileName();
}

// ��t�@�C�����H
bool CDocumentPane::isUntitled() const
{
	return QFileInfo(m_filePath).baseName().isEmpty();
}
