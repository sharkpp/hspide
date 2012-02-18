#include <QtGui>
#include "documentpane.h"

//////////////////////////////////////////////////////////////////////

CDocumentPane::CDocumentPane(QWidget *parent)
	: QWidget(parent)
	, m_editorWidget(new CCodeEdit(this))
	, m_highlighter(new CHsp3Highlighter(m_editorWidget->document()))
	, m_assignItem(NULL)
{
	connect(m_editorWidget, SIGNAL(pressIconArea(int)), this, SLOT(onPressEditorIconArea(int)));
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
		// �u���[�N�|�C���g���Z�b�g
		if( m_assignItem )
		{
			m_assignItem->setBreakPoint(lineNo + 1); // 1�I���W��
		}
	}
	else
	{
		m_editorWidget->clearLineIcon(lineNo);
		// �u���[�N�|�C���g�����Z�b�g
		if( m_assignItem )
		{
			m_assignItem->clearBreakPoint(lineNo + 1); // 1�I���W��
		}
	}

}

// �ݒ�X�V
void CDocumentPane::setConfiguration(const Configuration& info)
{
	connect(&info, SIGNAL(updateConfiguration(const Configuration&)),
	        this,  SLOT(updateConfiguration(const Configuration&)));
	updateConfiguration(info);
}

void CDocumentPane::updateConfiguration(const Configuration& info)
{
	QFont editorFont;
	Configuration::ColorMetricsInfoType metrics;

	// �t�H���g��ύX
	editorFont.setFamily(info.editorFontName());
	editorFont.setPixelSize(info.editorFontSize());
	editorFont.setFixedPitch(true);
	m_editorWidget->setFont(editorFont);

	// �s�ԍ��t�H���g��ύX
	editorFont.setFamily(info.editorLineNumberFontName());
	editorFont.setPixelSize(info.editorLineNumberFontSize());
	editorFont.setFixedPitch(true);
	m_editorWidget->setLineNumberFont(editorFont);

	m_editorWidget->setLineNumberVisible(info.editorLineNumberVisibled());
	m_editorWidget->setTabStopCharWidth(info.editorTabWidth());

	// �F��ύX
	metrics = info.colorMetrics(Configuration::LineNumberMetrics);
	m_editorWidget->setLineNumberTextColor(metrics.foregroundColor);
	m_editorWidget->setLineNumberBackgroundColor(metrics.backgroundColor);

	QPalette palette = m_editorWidget->palette();
	palette.setColor(QPalette::Window, metrics.backgroundColor);
	metrics = info.colorMetrics(Configuration::TextMetrics);
	palette.setColor(QPalette::Text, metrics.foregroundColor);
	palette.setColor(QPalette::Base, metrics.backgroundColor);
	m_editorWidget->setPalette(palette);

	// �L�[���[�h�̐F��ύX
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

	// �L�[���[�h�n�C���C�g���ĕ`��
	m_highlighter->rehighlight();
}

// �V���{���ꗗ���w��
void CDocumentPane::setSymbols(const QVector<QStringList> & symbols)
{
	m_editorWidget->setSymbols(symbols);
	m_highlighter->setSymbols(symbols);
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

	m_editorWidget->setPlainText(file.readAll());

	// �ύX�ʒm�V�O�i���ɐڑ�
	connect(m_editorWidget->document(), SIGNAL(modificationChanged(bool)), this, SLOT(onModificationChanged(bool)));

	return true;
}

// �t�@�C���ɕۑ�
bool CDocumentPane::save(const QString & filePath)
{
	QString fileName = filePath;

	// �����Ƀt�@�C�������w�肳��Ă����ꍇ�͂��̃t�@�C���ɕۑ�
	// �w�肳��Ă��Ȃ������ꍇ�́A
	// (����)�Ŗ����ꍇ�͂��̃t�@�C���ɕۑ�
	// (����)�̏ꍇ�̓t�@�C���w��_�C�A���O��\�����w��

	if( fileName.isEmpty() )
	{
		fileName = m_filePath;
		// ����̏ꍇ�͓K���ȃt�@�C������t����
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
				// �ێ����Ă���t�@�C�������V���Ɏw�肳�ꂽ�t�@�C�����������ꍇ
				// �ۑ��ł��Ȃ��̂ŃG���[�Ƃ���
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
	out << m_editorWidget->toPlainText();

	return true;
}

// �s�Ɉړ�
bool CDocumentPane::jump(int lineNo)
{
	QTextCursor cursor = m_editorWidget->textCursor();
	cursor.setPosition(0, QTextCursor::MoveAnchor);
	cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineNo - 1);
	cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
	m_editorWidget->setTextCursor(cursor);
	return true;
}

// �A�C�e���Ɗ֘A�t��
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
		m_editorWidget->document()->isModified();
}