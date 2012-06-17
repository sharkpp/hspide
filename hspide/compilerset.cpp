#include <QDebug>
#include <QDir>
#include <QLocalSocket>
#include <QProcessEnvironment>
#include "compilerset.h"
#include "compiler.h"
#include "debugger.h"
#include "workspaceitem.h"
#include "documentpane.h"

CCompilerSet::CCompilerSet(QObject *parent)
	: QObject(parent)
	, m_countOfSuccessful(0)
	, m_countOfFailure(0)
{
	QUuid uuid = QUuid::createUuid();

	m_server = new QLocalServer(this);
	if( !m_server->listen("hspide." + uuid.toString()) ) {
	}

	connect(m_server, SIGNAL(newConnection()), this, SLOT(attachDebugger()));

	setCompilerSlotNum(1);
}

// �R���p�C���X���b�g����ݒ�
bool CCompilerSet::setCompilerSlotNum(int num)
{
	for(int i = m_compilers.size(); num < i; i--) {
		m_compilers.pop_back();
	}

	for(int i = m_compilers.size(); i < num; i++) {
		m_compilers.push_back(new CCompiler(this));
	}

	// �X���b�g�Ɋ֘A�t�������ăV�O�i�����󂯎���悤��
	foreach(CCompiler* compiler, m_compilers)
	{
		connect(compiler, SIGNAL(compileStarted(QString)), this, SLOT(compileStarted(QString)));
		connect(compiler, SIGNAL(compileFailure()),        this, SLOT(compileFailure()));
		connect(compiler, SIGNAL(compileSuccessful()),     this, SLOT(compileSuccessful()));
		connect(compiler, SIGNAL(compileOutput(QString)),  this, SLOT(compileReadOutput(QString)));
	}

	return true;
}

// �R���p�C���X���b�g�����擾
int CCompilerSet::compilerSlotNum() const
{
	return m_compilers.size();
}

CWorkSpaceItem* CCompilerSet::getTargetItem(const QString& id) const
{
	foreach(const ExecOptionType& opt, m_execItems)
	{
		if( QString("hspide.%1.%2").arg(m_server->serverName()).arg(opt.uuid.toString())
				== id )
		{
			return opt.item;
		}
	}
	return NULL;
}

// �r���h�҂��̃A�C�e������R���p�C���Ɋ֘A�t��
bool CCompilerSet::assignBuildItem(CCompiler* compiler)
{
	bool result = false;
	if( !m_buildItems.isEmpty() )
	{
		BuildOptionType& opt = m_buildItems.front();
		m_buildingItems.push_back(opt);
		if( !(result = compiler->compile(opt.item, opt.debugMode)) ) {
			m_buildingItems.pop_back();
		}
		m_buildItems.pop_front();
	}
	return result;
}

// �R���p�C���Ɋ֘A�t�����Ă���r���h�I�v�V�������擾
const CCompilerSet::BuildOptionType* CCompilerSet::searchBuildOption(CCompiler* compiler, int* index) const
{
	for(int i = 0, num = m_buildingItems.size();
		i < num; i++)
	{
		const BuildOptionType& opt = m_buildingItems[i];

		if( opt.item == compiler->compileItem() )
		{
			if( index ) {
				*index = i;
			}
			return &opt;
		}
	}

	return NULL;
}

const CCompilerSet::ExecOptionType* CCompilerSet::searchExecOption(QProcess* process, int* index) const
{
	for(int i = 0, num = m_execItems.size();
		i < num; i++)
	{
		const ExecOptionType& opt = m_execItems[i];

		if( opt.process == process )
		{
			if( index ) {
				*index = i;
			}
			return &opt;
		}
	}

	return NULL;
}

// �r���h
bool CCompilerSet::build(CWorkSpaceItem* targetItem, bool debugMode)
{
	return execCompiler(targetItem, debugMode, false);
}

// �r���h����s
bool CCompilerSet::run(CWorkSpaceItem* targetItem, bool debugMode)
{
	return execCompiler(targetItem, debugMode, true);
}

// �r���h���~
bool CCompilerSet::buildCancel()
{
	if( m_buildItems.empty() &&
		m_buildingItems.empty() )
	{
		return false;
	}

	return true;
}

// �r���h�����s�̎�������
bool CCompilerSet::execCompiler(CWorkSpaceItem* targetItem, bool debugMode, bool buildAfterRun)
{
	if( !m_buildItems.empty() ||
		!m_buildingItems.empty() )
	{
		return false;
	}

	emit buildStarted();

	m_execItems.clear();
	m_countOfFailure    =
	m_countOfSuccessful = 0;

	// �r���h�ΏۃA�C�e�����
	switch( targetItem->type() )
	{
	case CWorkSpaceItem::Solution: {
		for(int i = 0, num = targetItem->count();
			i < num; i++ )
		{
			BuildOptionType opt = {
					targetItem->at(i),
					debugMode,
					buildAfterRun,
					m_buildItems.size(),
				};
			m_buildItems.push_back(opt);
		}
		break; }
	case CWorkSpaceItem::Project: {
		BuildOptionType opt = {
				targetItem,
				debugMode,
				buildAfterRun,
				m_buildItems.size(),
			};
		m_buildItems.push_back(opt);
		break; }
	default:
		emit buildFailure();
		return false;
	}

	// �A�C�e�����R���p�C���v���Z�X�Ɋ��蓖��
	foreach(CCompiler* compiler, m_compilers)
	{
		assignBuildItem(compiler);
	}

	return true;
}

bool CCompilerSet::execBuildDeliverables()
{
	QString workDir = QDir::currentPath(); // ����Ȃ�����

	typedef QPair<QStringList,QProcess*> ExecProcesseInfo;

	QList<QPair<QStringList,QProcess*> > execProcesses;

	for(QMutableListIterator<ExecOptionType> i(m_execItems);
		i.hasNext(); )
	{
		ExecOptionType& opt = i.next();
		QString program;
		QStringList arguments;

		opt.process = new QProcess(this);

		if( opt.debugMode )
		{
			// �f�o�b�K�Ƃ̒ʐM�p�̎��ʎq��o�^
			QProcessEnvironment env
				= QProcessEnvironment::systemEnvironment();
			env.insert("hspide.",
			           m_server->serverName() + "." +
			           opt.uuid.toString());
			opt.process->setProcessEnvironment(env);
			//
			arguments.push_front("");
		}
		else
		{
			arguments.push_front("");
		}

		// �����������̒ʒm��o�^
		connect(opt.process, SIGNAL(error(QProcess::ProcessError)),      this, SLOT(execError(QProcess::ProcessError)));
		connect(opt.process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(execFinished(int,QProcess::ExitStatus)));

		opt.process->setWorkingDirectory(workDir);

		execProcesses.push_back(qMakePair(arguments, opt.process));
	}

	emit debugStarted();

	// �N���r���ŃV�O�i�����Ă΂��m_execItems�̒��g���ς��ꍇ������̂�
	for(QMutableListIterator<ExecProcesseInfo> i(execProcesses);
		i.hasNext(); )
	{
		ExecProcesseInfo& execInfo = i.next();
		QStringList& arguments = execInfo.first;
		QString      program   = arguments.front();
		arguments.pop_front();
		execInfo.second->start(program, arguments);
	}

	return true;
}

void CCompilerSet::compileStarted(const QString& filePath)
{
	if( CCompiler* compiler = qobject_cast<CCompiler*>(sender()) )
	{
		if( const BuildOptionType* opt = searchBuildOption(compiler) ) {
			emit buildStarted(opt->buildOrder, filePath);
		}
	}
}

void CCompilerSet::compileSuccessful()
{
	if( CCompiler* compiler = qobject_cast<CCompiler*>(sender()) )
	{
		int index = -1;
		if( const BuildOptionType* opt = searchBuildOption(compiler, &index) )
		{
			emit buildSuccessful(opt->buildOrder);
			// �r���h�����X�g�����菜��
			if( opt->buildAfterRun )
			{
				ExecOptionType optExec = {
						opt->item,
						opt->debugMode,
						QUuid::createUuid(),
					};
				m_execItems.push_back(optExec);
			}
			m_buildingItems.removeAt(index);
			m_countOfSuccessful++;
			// ����ۂɂȂ�����r���h���s�Ŋ���
			assignBuildItem(compiler);
			if( m_buildingItems.isEmpty() )
			{
				if( m_countOfFailure ) {
					emit buildFailure();
				} else {
					emit buildSuccessful();
					// ���s
					execBuildDeliverables();
				}
			}
		}
	}
}

void CCompilerSet::compileFailure()
{
	if( CCompiler* compiler = qobject_cast<CCompiler*>(sender()) )
	{
		int index = -1;
		if( const BuildOptionType* opt = searchBuildOption(compiler, &index) )
		{
			emit buildFailure(opt->buildOrder);
			// �r���h�����X�g�����菜��
			m_buildingItems.removeAt(index);
			m_countOfFailure++;
			// ����ۂɂȂ�����r���h���s�Ŋ���
			assignBuildItem(compiler);
			if( m_buildingItems.isEmpty() ) {
				emit buildFailure();
			}
		}
	}
}

// �r���h���̏o�͂��擾
void CCompilerSet::compileReadOutput(const QString& output)
{
	if( CCompiler* compiler = qobject_cast<CCompiler*>(sender()) )
	{
		if( const BuildOptionType* opt = searchBuildOption(compiler) ) {
			emit buildOutput(opt->buildOrder, output);
		}
	}
}

void CCompilerSet::execError(QProcess::ProcessError)
{
	if( QProcess* process = qobject_cast<QProcess*>(sender()) )
	{
		int index = -1;
		if( const ExecOptionType* opt = searchExecOption(process, &index) )
		{
			m_execItems.removeAt(index);
			// ����ۂ�������f�o�b�O�I��
			if( m_execItems.isEmpty() ) {
				emit debugFinished();
			}
		}
	}
}

void CCompilerSet::execFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if( QProcess* process = qobject_cast<QProcess*>(sender()) )
	{
		int index = -1;
		if( const ExecOptionType* opt = searchExecOption(process, &index) )
		{
			m_execItems.removeAt(index);
			// ����ۂ�������f�o�b�O�I��
			if( m_execItems.isEmpty() ) {
				emit debugFinished();
			}
		}
	}
}

void CCompilerSet::attachDebugger()
{
	QLocalSocket* clientConnection = m_server->nextPendingConnection();

	// �ؒf���ꂽ�珟��ɍ폜
	connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));

	// �f�o�b�K�Ɗ֘A�t��
	CDebugger* debugger = new CDebugger(this, clientConnection);

	emit attachedDebugger(debugger);
}