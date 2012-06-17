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

// コンパイラスロット数を設定
bool CCompilerSet::setCompilerSlotNum(int num)
{
	for(int i = m_compilers.size(); num < i; i--) {
		m_compilers.pop_back();
	}

	for(int i = m_compilers.size(); i < num; i++) {
		m_compilers.push_back(new CCompiler(this));
	}

	// スロットに関連付けをしてシグナルを受け取れるように
	foreach(CCompiler* compiler, m_compilers)
	{
		connect(compiler, SIGNAL(compileStarted(QString)), this, SLOT(compileStarted(QString)));
		connect(compiler, SIGNAL(compileFailure()),        this, SLOT(compileFailure()));
		connect(compiler, SIGNAL(compileSuccessful()),     this, SLOT(compileSuccessful()));
		connect(compiler, SIGNAL(compileOutput(QString)),  this, SLOT(compileReadOutput(QString)));
	}

	return true;
}

// コンパイラスロット数を取得
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

// ビルド待ちのアイテムからコンパイラに関連付け
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

// コンパイラに関連付けられているビルドオプションを取得
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

// ビルド
bool CCompilerSet::build(CWorkSpaceItem* targetItem, bool debugMode)
{
	return execCompiler(targetItem, debugMode, false);
}

// ビルド後実行
bool CCompilerSet::run(CWorkSpaceItem* targetItem, bool debugMode)
{
	return execCompiler(targetItem, debugMode, true);
}

// ビルド中止
bool CCompilerSet::buildCancel()
{
	if( m_buildItems.empty() &&
		m_buildingItems.empty() )
	{
		return false;
	}

	return true;
}

// ビルド＆実行の実装処理
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

	// ビルド対象アイテムを列挙
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

	// アイテムをコンパイラプロセスに割り当て
	foreach(CCompiler* compiler, m_compilers)
	{
		assignBuildItem(compiler);
	}

	return true;
}

bool CCompilerSet::execBuildDeliverables()
{
	QString workDir = QDir::currentPath(); // いらないかも

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
			// デバッガとの通信用の識別子を登録
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

		// 処理完了時の通知を登録
		connect(opt.process, SIGNAL(error(QProcess::ProcessError)),      this, SLOT(execError(QProcess::ProcessError)));
		connect(opt.process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(execFinished(int,QProcess::ExitStatus)));

		opt.process->setWorkingDirectory(workDir);

		execProcesses.push_back(qMakePair(arguments, opt.process));
	}

	emit debugStarted();

	// 起動途中でシグナルが呼ばれてm_execItemsの中身が変わる場合があるので
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
			// ビルド中リストから取り除く
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
			// 空っぽになったらビルド失敗で完了
			assignBuildItem(compiler);
			if( m_buildingItems.isEmpty() )
			{
				if( m_countOfFailure ) {
					emit buildFailure();
				} else {
					emit buildSuccessful();
					// 実行
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
			// ビルド中リストから取り除く
			m_buildingItems.removeAt(index);
			m_countOfFailure++;
			// 空っぽになったらビルド失敗で完了
			assignBuildItem(compiler);
			if( m_buildingItems.isEmpty() ) {
				emit buildFailure();
			}
		}
	}
}

// ビルド中の出力を取得
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
			// 空っぽだったらデバッグ終了
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
			// 空っぽだったらデバッグ終了
			if( m_execItems.isEmpty() ) {
				emit debugFinished();
			}
		}
	}
}

void CCompilerSet::attachDebugger()
{
	QLocalSocket* clientConnection = m_server->nextPendingConnection();

	// 切断されたら勝手に削除
	connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));

	// デバッガと関連付け
	CDebugger* debugger = new CDebugger(this, clientConnection);

	emit attachedDebugger(debugger);
}
