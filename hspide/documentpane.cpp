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

	m_breakPointIcon = m_editorWidget->registLineIcon(QIcon(":/images/icons/small/breakpoint.png"));
	m_cursorIcon     = m_editorWidget->registLineIcon(QIcon(":/images/tango/small/go-next.png"));

	// �ݒ�̕ύX�ʎj�̓o�^�Ɛݒ肩��̏���������
	connect(theConf, SIGNAL(updateConfiguration(const Configuration*)),
	        this,  SLOT(updateConfiguration(const Configuration*)));
	updateConfiguration(theConf);

	connect(theBreakPoint, SIGNAL(breakPointChanged(const QUuid&, const QList<QPair<int, bool> >&)),
	        this,  SLOT(onBreakPointChanged(const QUuid&, const QList<QPair<int, bool> >&)));
}

void CDocumentPane::resizeEvent(QResizeEvent* event)
{
	m_editorWidget->resize(event->size());
}

void CDocumentPane::focusInEvent(QFocusEvent* /*event*/)
{
	m_editorWidget->setFocus();
}

void CDocumentPane::onModificationChanged(bool changed)
{
	emit modificationChanged(isUntitled() | changed);
}

void CDocumentPane::onPressEditorIconArea(int lineNo)
{
	if( !m_editorWidget->lineIcon(lineNo, m_breakPointIcon) &&
		!m_uuid.isNull() )
	{
		// �u���[�N�|�C���g���Z�b�g
		theBreakPoint->append(m_uuid, lineNo + 1); // 1�I���W��
	}
	else
	{
		// �u���[�N�|�C���g�����Z�b�g
		theBreakPoint->remove(m_uuid, lineNo + 1); // 1�I���W��
	}

	emit updateBreakpoint();
}

void CDocumentPane::updateConfiguration(const Configuration* conf)
{
	QFont editorFont;
	Configuration::ColorMetricsInfoType metrics;

	// �t�H���g��ύX
	editorFont.setFamily(conf->editorFontName());
	editorFont.setPixelSize(conf->editorFontSize());
	editorFont.setFixedPitch(true);
	m_editorWidget->setFont(editorFont);

	// �s�ԍ��t�H���g��ύX
	editorFont.setFamily(conf->editorLineNumberFontName());
	editorFont.setPixelSize(conf->editorLineNumberFontSize());
	editorFont.setFixedPitch(true);
	m_editorWidget->setLineNumberFont(editorFont);

	m_editorWidget->setLineNumberVisible(conf->editorLineNumberVisibled());
	m_editorWidget->setTabStopCharWidth(conf->editorTabWidth());

	// �F��ύX
	metrics = conf->colorMetrics(Configuration::LineNumberMetrics);
	m_editorWidget->setLineNumberTextColor(metrics.foregroundColor);
	m_editorWidget->setLineNumberBackgroundColor(metrics.backgroundColor);

	QPalette palette = m_editorWidget->palette();
	palette.setColor(QPalette::Window, metrics.backgroundColor);
	metrics = conf->colorMetrics(Configuration::TextMetrics);
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
					<< qMakePair(Configuration::NumberMetrics,       Hsp3Lexer::TypeNumber)
					;
	for(int i = 0; i < keywordFormat.size(); i++)
	{
		QTextCharFormat format;
		Hsp3Lexer::Type type = keywordFormat[i].second;
		metrics = conf->colorMetrics(keywordFormat[i].first);
		format.setForeground(metrics.foregroundColor);
		format.setBackground(metrics.backgroundColor);
		format.setFontWeight(metrics.useBoldFont ? QFont::Bold : QFont::Normal);
		m_highlighter->setFormat(type, format);
	}

	// �L�[���[�h�n�C���C�g���ĕ`��
	m_highlighter->rehighlight();
}

void CDocumentPane::onBreakPointChanged(const QUuid& uuid, const QList<QPair<int, bool> >& modifiedLineNumberes)
{
	if( m_uuid != uuid ) {
		return;
	}

	for(QList<QPair<int, bool> >::const_iterator
			ite = modifiedLineNumberes.begin(),
			last= modifiedLineNumberes.end();
		ite != last; ++ite)
	{
		if( ite->second ) {
			m_editorWidget->setLineIcon(ite->first - 1, m_breakPointIcon);
		} else {
			m_editorWidget->clearLineIcon(ite->first - 1, m_breakPointIcon);
		}
	}
}

// �V���{���ꗗ���w��
void CDocumentPane::setSymbols(const QVector<QStringList>& symbols)
{
	m_editorWidget->setSymbols(symbols);
	m_highlighter->setSymbols(symbols);
}

// �t�@�C������ǂݍ���
bool CDocumentPane::load(const QString& filepath, const QString& tmplFilePath)
{
	QFile file(tmplFilePath.isEmpty() ? filepath : tmplFilePath);

	if( !file.open(QFile::ReadOnly | QFile::Text) )
	{
		return false;
	}

	if( !m_uuid.isNull() )
	{
		theFile->rename(m_uuid, filepath);
	}
	else
	{
		m_uuid = theFile->assign(filepath);
	}

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
bool CDocumentPane::save(const QString& filePath, bool saveAs, bool temporary)
{
	QString fileName = filePath;
	QString lastFilePath = theFile->path(m_uuid);

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
								? tr("untitled") + (fileInfo.suffix().isEmpty() ? "" : "." + fileInfo.suffix())
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

	if( !temporary )
	{
		m_editorWidget->document()->setModified(false);
		theFile->rename(m_uuid, fileName);
	}

	return true;
}

// �w��s�Ɉړ�
bool CDocumentPane::jump(int lineNo)
{
	QTextCursor cursor = m_editorWidget->textCursor();
	cursor.setPosition(0, QTextCursor::MoveAnchor);
	cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineNo - 1);
	m_editorWidget->setTextCursor(cursor);
	setFocus();
	return true;
}

// �A�C�e���Ɗ֘A�t��
bool CDocumentPane::setAssignItem(CWorkSpaceItem* item)
{
	if( m_assignItem ) {
		m_assignItem->setAssignDocument(NULL);
	}
	m_assignItem = item;
	m_assignItem->setAssignDocument(this);
	m_uuid = m_assignItem->uuid();
	return true;
}

CWorkSpaceItem* CDocumentPane::assignItem()
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
	return theFile->path(m_uuid);
}

// �^�C�g�����擾
QString CDocumentPane::title() const
{
	return theFile->fileName(m_uuid) + (m_editorWidget->document()->isModified() ? "*" : "");
}

// ����t�@�C��(�f�B�X�N�ɕۑ����Ă��Ȃ��t�@�C��)���H
bool CDocumentPane::isUntitled() const
{
	return theFile->isUntitled(m_uuid);
}

// �ύX����Ă��邩
bool CDocumentPane::isModified() const
{
	return
		m_editorWidget->document() &&
		m_editorWidget->document()->isModified();
}

void CDocumentPane::markCursorLine(const QUuid& uuid, int lineNo)
{
	if( m_uuid != uuid ) {
		return;
	}

	m_editorWidget->clearLineIcon(-1, m_cursorIcon);
	m_editorWidget->setLineIcon(lineNo - 1, m_cursorIcon);
}

void CDocumentPane::unmarkCursorLine()
{
	m_editorWidget->clearLineIcon(-1, m_cursorIcon);
}
