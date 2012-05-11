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
{
	updateCompilerPath();

	m_server = new QLocalServer(this);
	if( !m_server->listen("test@hspide") ) {
	}
	connect(m_server, SIGNAL(newConnection()), this, SLOT(attachDebugger()));

	// �ݒ�̕ύX�ʎj�̓o�^�Ɛݒ肩��̏���������
	connect(&theConf, SIGNAL(updateConfiguration(const Configuration&)),
	        this,  SLOT(updateConfiguration(const Configuration&)));
	updateConfiguration(theConf);
}

void CCompiler::updateConfiguration(const Configuration& info)
{
	bool update = false;
	QString hspPath       = QDir::toNativeSeparators(info.hspPath());
	QString hspCompPath   = QDir::toNativeSeparators(info.compilerPath());
	QString hspCommonPath = QDir::toNativeSeparators(info.hspCommonPath());
	update |= m_hspPath       != hspPath;
	update |= m_hspCompPath   != hspCompPath;
	update |= m_hspCommonPath != hspCommonPath;
	m_hspCompPath   = hspPath;
	m_hspPath       = hspCompPath;
	m_hspCommonPath = hspCommonPath;
	if( update ) {
		updateCompilerPath();
	}
}

CWorkSpaceItem* CCompiler::getTargetItem(qint64 id)
{
	QMap<quint64, CWorkSpaceItem*>::iterator
		ite = m_targetsTemp.find(id);
	if( m_targetsTemp.end() == ite )
	{
		return NULL;
	}

	CWorkSpaceItem* item = *ite;

	m_targetsTemp.erase(ite);

	return item;
}

void CCompiler::updateCompilerPath()
{
	QString program = m_hspCompPath + "hspcmp.exe";
	QStringList arguments;
	arguments << "--symbol-put";

	if( !QFileInfo(program).exists() )
	{
		return;
	}

	QProcess* listingSymbolsProcess = new QProcess(this);

	listingSymbolsProcess->start(program, arguments);

	if( !listingSymbolsProcess->waitForStarted() ) {
		delete listingSymbolsProcess;
		return;
	}

	// �����������̒ʒm��o�^
	connect(listingSymbolsProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
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

	QProcess* proccess = new QProcess(this);

	quint64 id = (quint64)proccess;

	m_targetsTemp[id] = targetItem; // �r���Ŏ��s�����ꍇ���[�N���邯�ǂǂ�����ĉ������悤�H

	QString program = m_hspCompPath + "hspcmp";
	QStringList arguments;
	arguments << "-C" << m_hspCommonPath
	          << "-H" << m_hspPath;
	if( debugMode ) { // �f�o�b�O���[�h
		arguments << "-d";
	}
	if( tempSave ) { // �ꎞ�I�ȃt�@�C���ɕۑ�
		arguments << "-o" << "obj"
		          << "-r" << QString("?%1").arg(targetItem->uuid().toString());
	} else {
		arguments << "-r" << QString("?%1").arg(targetItem->uuid().toString());
	}
	if( buildAfterRun ) { // �r���h��Ɏ��s
		arguments << "-e";
	}
	if( buildAfterRun && debugMode ) { // �f�o�b�K�ŃA�^�b�`��ҋ@
		arguments << "--attach"
		          << QString("%1").arg(id, 0, 16);
	}
	arguments << filename;
qDebug() << arguments;

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
	QProcess* listingSymbolsProcess = qobject_cast<QProcess*>(sender());
	QString tmp(listingSymbolsProcess->readAllStandardOutput());

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

	emit updatedSymbols();

	listingSymbolsProcess->deleteLater();
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

	emit attachedDebugger(debugger);
}
