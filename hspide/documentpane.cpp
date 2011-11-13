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

// �V���{���ꗗ���w��
void CDocumentPane::setSymbols(const QVector<QStringList> & symbols)
{
	editorWidget->setSymbols(symbols);
}

// �t�@�C������ǂݍ���
bool CDocumentPane::load(const QString & filepath, const QString & tmplFilePath)
{
	QFile file(tmplFilePath.isEmpty() ? filepath : tmplFilePath);

	if( !file.open(QFile::ReadOnly | QFile::Text) )
	{
		return false;
	}

	m_filePath = filepath;

	editorWidget->setPlainText(file.readAll());

	// �ύX�ʒm�V�O�i���ɐڑ�
	connect(editorWidget->document(), SIGNAL(modificationChanged(bool)), this, SLOT(onModificationChanged(bool)));

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
	out << editorWidget->toPlainText();

	return true;
}

// �A�C�e���Ɗ֘A�t��
bool CDocumentPane::setAssignItem(CWorkSpaceItem * item)
{
	m_assignItem = item;
	return true;
}

CWorkSpaceItem * CDocumentPane::assignItem()
{
	return m_assignItem;
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

// �ύX����Ă��邩
bool CDocumentPane::isModified() const
{
	return
		isUntitled() ||
		editorWidget->document()->isModified();
}