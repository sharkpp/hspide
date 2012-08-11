#include <QDebug>
#include <QDir>
#include <QLocalSocket>
#include <QProcessEnvironment>
#include <QTemporaryFile>
#include <QRegExp>
#include "global.h"
#include "compilerset.h"
#include "compiler.h"
#include "debugger.h"
#include "workspaceitem.h"

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

	// 設定の変更通史の登録と設定からの初期化処理
	connect(theConf, SIGNAL(updateConfiguration(const Configuration*)),
	        this,  SLOT(updateConfiguration(const Configuration*)));
	updateConfiguration(theConf);
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
		connect(compiler, SIGNAL(compileStarted(QUuid)),  this, SLOT(compileStarted(QUuid)));
		connect(compiler, SIGNAL(compileFailure()),       this, SLOT(compileFailure()));
		connect(compiler, SIGNAL(compileSuccessful()),    this, SLOT(compileSuccessful()));
		connect(compiler, SIGNAL(compileOutput(QString)), this, SLOT(compileReadOutput(QString)));
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
		if( opt.uuid.toString() == id )
		{
			return opt.item;
		}
	}
	return NULL;
}

// エラー文字列を取得
QString CCompilerSet::hspErrorText(int errorNo)
{
	if( !m_compilers.isEmpty() )
	{
		QStringList hspErrorTexts = m_compilers.front()->hspErrorTexts();
		if( 0 <= errorNo && errorNo < hspErrorTexts.size() )
		{
			return hspErrorTexts[errorNo];
		}
	}

	return tr("Unknown error");
}

// ビルド待ちのアイテムからコンパイラに関連付け
bool CCompilerSet::assignBuildItem(CCompiler* compiler)
{
	bool result = false;
	if( !m_buildItems.isEmpty() )
	{
		BuildOptionType& opt = m_buildItems.front();
		m_buildingItems.push_back(opt);
		if( !(result = compiler->compile(opt.item)) ) {
			m_buildingItems.pop_back();
		} else {
			m_buildingItems.back().objTemp
				 = compiler->objTemp();
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

		Configuration::BuildConfType
				buildConf = opt.item->currentBuildConf();

		if( buildConf.noExecute )
		{
			continue;
		}

		opt.process = new QProcess(this);

		// 親を置き換えてプロセスが動作しているときのみ存在するようにする
		if( opt.objTemp )
		{
			opt.objTemp->setParent(opt.process);
		}

		if( buildConf.debug )
		{
			// デバッガとの通信用の識別子を登録
			QProcessEnvironment env
				= QProcessEnvironment::systemEnvironment();
			env.insert("hspide",
			           m_server->serverName() + "." +
			           opt.uuid.toString());
			opt.process->setProcessEnvironment(env);
		}
		// 実行ファイルを登録
		if( !opt.objName.isEmpty() ) {
			arguments.push_front(opt.objName);
		}
		if( QFileInfo(opt.runtime).isAbsolute() ) {
			arguments.push_front(opt.runtime);
		} else {
			arguments.push_front(QDir(m_hspPath).absoluteFilePath(opt.runtime));
		}
		arguments.push_front("--thunk-call");
		arguments.push_front(QDir::toNativeSeparators(QDir(m_hspCompPath).absoluteFilePath("hspcmp.exe")));

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
qDebug() << execInfo.first;
		QStringList& arguments = execInfo.first;
		QString      program   = arguments.front();
		arguments.pop_front();
		execInfo.second->start(program, arguments);
	}

	return true;
}

void CCompilerSet::updateConfiguration(const Configuration* conf)
{
	m_hspPath     = QDir::toNativeSeparators(conf->hspPath());
	m_hspCompPath = QDir::toNativeSeparators(conf->compilerPath());
}

void CCompilerSet::compileStarted(const QUuid& uuid)
{
	if( CCompiler* compiler = qobject_cast<CCompiler*>(sender()) )
	{
		if( const BuildOptionType* opt = searchBuildOption(compiler) ) {
			emit buildStarted(opt->buildOrder, uuid);
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
						QUuid::createUuid(),
						opt->runtime,
						opt->objName,
						opt->objTemp,
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
		int index = -1;
		if( const BuildOptionType* opt = searchBuildOption(compiler, &index) )
		{
			emit buildOutput(opt->buildOrder, output);
			// ランタイムを取得
			QRegExp reRtm("#runtime\\s+'([^']+)'");
			if( 0 <= reRtm.indexIn(output) ) {
				m_buildingItems[index].runtime = reRtm.cap(1);
			}
			QRegExp reObj("#objname\\s+'([^']+)'");
			if( 0 <= reObj.indexIn(output) ) {
				m_buildingItems[index].objName = reObj.cap(1);
			}
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
		process->deleteLater();
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
		process->deleteLater();
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
