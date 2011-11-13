#include <QDebug>
#include <QDir>
#include "compiler.h"
#include "workspaceitem.h"

CCompiler::CCompiler(QObject *parent)
	: QObject(parent)
	, m_hspCompPath(QDir::toNativeSeparators("./"))
	, m_hspPath(QDir::toNativeSeparators("./"))
	, m_hspCommonPath(QDir::toNativeSeparators("./common/"))
	, m_compilerProcess(NULL)
	, m_listingSymbolsProcess(NULL)
{
	updateCompilerPath();
}

// �R���p�C���̃p�X���擾
const QString &  CCompiler::compilerPath() const
{
	return m_hspCompPath;
}

// �R���p�C���̃p�X���w��
void CCompiler::setCompilerPath(const QString & path)
{
	m_hspCompPath = QDir::toNativeSeparators(path);
	updateCompilerPath();
}

// HSP�f�B���N�g���̃p�X���擾
const QString &  CCompiler::hspPath() const
{
	return m_hspPath;
}

// HSP�f�B���N�g���̃p�X���w��
void CCompiler::setHspPath(const QString & path)
{
	m_hspPath = QDir::toNativeSeparators(path);
}

// HSP�f�B���N�g���̃p�X���擾
const QString &  CCompiler::hspCommonPath() const
{
	return m_hspCommonPath;
}

// HSP common�f�B���N�g���̃p�X���w��
void CCompiler::setHspCommonPath(const QString & path)
{
	m_hspCommonPath = QDir::toNativeSeparators(path);
}

void CCompiler::updateCompilerPath()
{
	QString program = m_hspCompPath + "hspcmp";
	QStringList arguments;
	arguments << "--symbol-put";

	delete m_listingSymbolsProcess;
	m_listingSymbolsProcess = new QProcess(this);

	m_listingSymbolsProcess->start(program, arguments);

	if( !m_listingSymbolsProcess->waitForStarted() ) {
		return;
	}

	// �����������̒ʒm��o�^
	connect(m_listingSymbolsProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(listedSymbolsFinished(int,QProcess::ExitStatus)));
}

// �V���{���ꗗ���擾
const QVector<QStringList> & CCompiler::symbols() const
{
	return m_highlightSymbols;
}

// �v���W�F�N�g���r���h
void CCompiler::build(CWorkSpaceItem * project)
{
	QString filename;
	//if( !project->getMainSource(filename) )
	{
		return;
	}

	QString program = m_hspCompPath + "hspcmp";
	QStringList arguments;
	arguments << "-C" << m_hspCommonPath
	          << "-H" << m_hspPath
	          << filename;
	delete m_compilerProcess;
	m_compilerProcess = new QProcess(this);
	m_compilerProcess->start(program, arguments);

	if( !m_compilerProcess->waitForStarted() ) {
		emit buildFinished(false);
		delete m_compilerProcess;
		return;
	}

	// �����������̒ʒm��o�^
	connect(m_compilerProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(buildFinished(int,QProcess::ExitStatus)));
	connect(m_compilerProcess, SIGNAL(readyReadStandardOutput()),          this, SLOT(buildReadOutput()));

	// �V�O�i������
	emit buildStart();
}

// �V���{���̎擾����
void CCompiler::listedSymbolsFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	QString tmp(m_listingSymbolsProcess->readAllStandardOutput());

	m_highlightSymbols.clear();

	for(QStringListIterator ite(tmp.split("\n"));
		ite.hasNext(); )
	{
		QString line = ite.next();
		
		if( !line.contains(",") ) {
			continue;
		}

		// �R���p�C���̃��b�Z�[�W�o�b�t�@��HSP�Ŏg�p�����V���{�������o�͂��܂��B
		// �o�͂́A�u�V���{����,sys[|var/func/macro][|1/2]�v�̌`���ɂȂ�܂��B

		QStringList keywords = line.trimmed().split(QRegExp(",|\\|"));

		for(QMutableStringListIterator ite2(keywords);
			ite2.hasNext(); )
		{
			QString & keyword = ite2.next();
			keyword = keyword.trimmed();
		}

		m_highlightSymbols.push_back(keywords);
	}

//	qDebug() << mSymbols;

	delete m_listingSymbolsProcess;
}

// �v���W�F�N�g�̃r���h����
void CCompiler::buildFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	// �V�O�i������
	emit buildFinished(QProcess::NormalExit == exitStatus);
	delete m_compilerProcess;
}

// �r���h���̏o�͂��擾
void CCompiler::buildReadOutput()
{
	QString tmp(m_compilerProcess->readAllStandardOutput());
	emit buildOutput(tmp);
}
