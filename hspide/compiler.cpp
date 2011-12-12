#include <QDebug>
#include <QDir>
#include <QLocalSocket>
#include "compiler.h"
#include "workspaceitem.h"
#include "documentpane.h"
#include "debugger.h"

CCompiler::CCompiler(QObject *parent)
	: QObject(parent)
	, m_hspCompPath(QDir::toNativeSeparators("./"))
	, m_hspPath(QDir::toNativeSeparators("./"))
	, m_hspCommonPath(QDir::toNativeSeparators("./common/"))
	, m_listingSymbolsProcess(NULL)
{
	updateCompilerPath();

	m_server = new QLocalServer(this);
	if( !m_server->listen("test@hspide") ) {
	}
	connect(m_server, SIGNAL(newConnection()), this, SLOT(attachDebugger()));
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
	connect(m_listingSymbolsProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
			this, SLOT(listedSymbolsFinished(int,QProcess::ExitStatus)));
}

// �V���{���ꗗ���擾
const QVector<QStringList> & CCompiler::symbols() const
{
	return m_highlightSymbols;
}

// �R���p�C���Ăяo��
bool CCompiler::execCompiler(CWorkSpaceItem * targetItem, bool buildAfterRun, bool debugMode)
{
	QString workDir = QDir::currentPath(); // ����Ȃ�����

	QString filename = targetItem->path();
	bool tempSave = false;

	// ����̏ꍇ�͈ꎞ�I�ȃt�@�C���ɕۑ�
	if( filename.isEmpty() &&
		targetItem->isUntitled() )
	{
		CDocumentPane* document = targetItem->assignDocument();
		if( !document )
		{
			emit buildStart(filename);
			emit buildFinished(false);
			return false;
		}

		// �ꎞ�I�ȃt�@�C���ɕۑ�
		tempSave = true;
		filename = QDir::toNativeSeparators(workDir + "/hsptmp");
		document->save(filename);
	}

	// �V�O�i������
	emit buildStart(filename);

	QString program = m_hspCompPath + "hspcmp";
	QStringList arguments;
	arguments << "-C" << m_hspCommonPath
	          << "-H" << m_hspPath;
	if( debugMode ) { // �f�o�b�O���[�h
		arguments << "-d";
	}
	if( tempSave ) { // �ꎞ�I�ȃt�@�C���ɕۑ�
		arguments << "-o" << "obj"
		          << "-r" << "???";
	}
	if( buildAfterRun ) { // �r���h��Ɏ��s
		arguments << "-e";
	}
	if( buildAfterRun && debugMode ) { // �f�o�b�K�ŃA�^�b�`��ҋ@
		arguments << "--attach"
		          << QString("%1").arg((quint64)0, 0, 16);
	}
	arguments << filename;

	QProcess* proccess = new QProcess(this);

	// �����������̒ʒm��o�^
	connect(proccess, SIGNAL(error(QProcess::ProcessError)),      this, SLOT(buildError(QProcess::ProcessError)));
	connect(proccess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(buildFinished(int,QProcess::ExitStatus)));
	connect(proccess, SIGNAL(readyReadStandardOutput()),          this, SLOT(buildReadOutput()));

	proccess->setWorkingDirectory(workDir);
	proccess->start(program, arguments);

	if( !proccess->waitForStarted() ) {
		delete proccess;
		emit buildFinished(false);
		return false;
	}

	m_compilerProcesses.append(proccess);

	return true;
}

// �r���h
bool CCompiler::build(CWorkSpaceItem * targetItem, bool debugMode)
{
	return execCompiler(targetItem, false, debugMode);
}

// ���s
bool CCompiler::run(CWorkSpaceItem * targetItem, bool debugMode)
{
	return execCompiler(targetItem, true, debugMode);
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
	m_listingSymbolsProcess = NULL;
}

// �v���W�F�N�g�̃r���h���s
void CCompiler::buildError(QProcess::ProcessError error)
{
}

// �v���W�F�N�g�̃r���h����
void CCompiler::buildFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	// �V�O�i������
	emit buildFinished(QProcess::NormalExit == exitStatus);

	// �r���h�������X�g�����菜��
	QProcess* compiler = qobject_cast<QProcess*>(sender());
	m_compilerProcesses.removeAll(compiler);
	delete compiler;
}

// �r���h���̏o�͂��擾
void CCompiler::buildReadOutput()
{
	QProcess* compiler = qobject_cast<QProcess*>(sender());
	QString tmp(compiler->readAllStandardOutput());
	emit buildOutput(tmp);
}

void CCompiler::attachDebugger()
{
	QLocalSocket* clientConnection = m_server->nextPendingConnection();

	// �ؒf���ꂽ�珟��ɍ폜
	connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));

	// �f�o�b�K�Ɗ֘A�t��
	CDebugger* debugger = new CDebugger(this, clientConnection);
}
