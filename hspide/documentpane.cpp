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

	// �ݒ�̕ύX�ʎj�̓o�^�Ɛݒ肩��̏���������
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

	emit updateBreakpoint();
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

	m_uuid = theFile.assign(filepath);

	m_editorWidget->setPlainText(file.readAll());

	// �e���v���[�g����ǉ��������͏������ɕύX����ɂ���
	if( !tmplFilePath.isEmpty() ) {
		m_editorWidget->document()->setModified();
	}

	// �ύX�ʒm�V�O�i���ɐڑ�
	connect(m_editorWidget->document(), SIGNAL(modificationChanged(bool)), this, SLOT(onModificationChanged(bool)));

	return true;
}

// �t�@�C���ɕۑ�
bool CDocumentPane::save(const QString & filePath, bool saveAs)
{
	QString fileName = filePath;
	QString lastFilePath = theFile.path(m_uuid);

	// �����Ƀt�@�C�������w�肳��Ă����ꍇ�͂��̃t�@�C���ɕۑ�
	// �w�肳��Ă��Ȃ������ꍇ�́A
	// (����)�Ŗ����ꍇ�͂��̃t�@�C���ɕۑ�
	// (����)�̏ꍇ�̓t�@�C���w��_�C�A���O��\�����w��

	if( fileName.isEmpty() || saveAs )
	{
		if( fileName.isEmpty() || !saveAs ) {
			fileName = lastFilePath;
		}
		// ����̏ꍇ�͓K���ȃt�@�C������t����
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
				// �ێ����Ă���t�@�C�������V���Ɏw�肳�ꂽ�t�@�C�����������ꍇ
				// �ۑ��ł��Ȃ��̂ŃG���[�Ƃ���
				return false;
			}

			fileName = QDir::toNativeSeparators(fileName);
		}
	}

	// �ύX���������Ă��邩�H
	if( lastFilePath == fileName &&
		!m_editorWidget->document()->isModified() )
	{
		// �ύX����Ă��Ȃ���΃t�@�C���ւ͕ۑ������ɖ߂�
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

// uuid���擾
const QUuid& CDocumentPane::uuid() const
{
	return m_uuid;
}

// �t�@�C���p�X���擾
QString CDocumentPane::path() const
{
	return theFile.path(m_uuid);
}

// �^�C�g�����擾
QString CDocumentPane::title() const
{
	return theFile.fileName(m_uuid) + (m_editorWidget->document()->isModified() ? "*" : "");
}

// ����t�@�C��(�f�B�X�N�ɕۑ����Ă��Ȃ��t�@�C��)���H
bool CDocumentPane::isUntitled() const
{
	return theFile.isUntitled(m_uuid);
}

// �ύX����Ă��邩
bool CDocumentPane::isModified() const
{
	return
		m_editorWidget->document() &&
		m_editorWidget->document()->isModified();
}