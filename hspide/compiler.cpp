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

	// 設定の変更通史の登録と設定からの初期化処理
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

	// 処理完了時の通知を登録
	connect(listingSymbolsProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
			this, SLOT(listedSymbolsFinished(int,QProcess::ExitStatus)));
}

// シンボル一覧を取得
const QVector<QStringList> & CCompiler::symbols() const
{
	return m_highlightSymbols;
}

// コンパイラ呼び出し
bool CCompiler::execCompiler(CWorkSpaceItem * targetItem, bool buildAfterRun, bool debugMode)
{
	QString workDir = QDir::currentPath(); // いらないかも

	QString filename = targetItem->path();
	bool tempSave = false;

	// 無題の場合は一時的なファイルに保存
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

		// 一時的なファイルに保存
		tempSave = true;
		filename = QDir::toNativeSeparators(workDir + "/hsptmp");
		document->save(filename);
	}

	// シグナル発報
	emit buildStart(filename);

	QProcess* proccess = new QProcess(this);

	quint64 id = (quint64)proccess;

	m_targetsTemp[id] = targetItem; // 途中で失敗した場合リークするけどどうやって解消しよう？

	QString program = m_hspCompPath + "hspcmp";
	QStringList arguments;
	arguments << "-C" << m_hspCommonPath
	          << "-H" << m_hspPath;
	if( debugMode ) { // デバッグモード
		arguments << "-d";
	}
	if( tempSave ) { // 一時的なファイルに保存
		arguments << "-o" << "obj"
		          << "-r" << QString("?%1").arg(targetItem->uuid().toString());
	} else {
		arguments << "-r" << QString("?%1").arg(targetItem->uuid().toString());
	}
	if( buildAfterRun ) { // ビルド後に実行
		arguments << "-e";
	}
	if( buildAfterRun && debugMode ) { // デバッガでアタッチを待機
		arguments << "--attach"
		          << QString("%1").arg(id, 0, 16);
	}
	arguments << filename;
qDebug() << arguments;

	// 処理完了時の通知を登録
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

// ビルド
bool CCompiler::build(CWorkSpaceItem * targetItem, bool debugMode)
{
	return execCompiler(targetItem, false, debugMode);
}

// 実行
bool CCompiler::run(CWorkSpaceItem * targetItem, bool debugMode)
{
	return execCompiler(targetItem, true, debugMode);
}

// シンボルの取得完了
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

		// コンパイラのメッセージバッファにHSPで使用されるシンボル名を出力します。
		// 出力は、「シンボル名,sys[|var/func/macro][|1/2]」の形式になります。

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

// プロジェクトのビルド失敗
void CCompiler::buildError(QProcess::ProcessError error)
{
}

// プロジェクトのビルド完了
void CCompiler::buildFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	// シグナル発報
	emit buildFinished(QProcess::NormalExit == exitStatus);

	// ビルド処理リストから取り除く
	QProcess* compiler = qobject_cast<QProcess*>(sender());
	m_compilerProcesses.removeAll(compiler);
	delete compiler;
}

// ビルド中の出力を取得
void CCompiler::buildReadOutput()
{
	QProcess* compiler = qobject_cast<QProcess*>(sender());
	QString tmp(compiler->readAllStandardOutput());
	emit buildOutput(tmp);
}

void CCompiler::attachDebugger()
{
	QLocalSocket* clientConnection = m_server->nextPendingConnection();

	// 切断されたら勝手に削除
	connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));

	// デバッガと関連付け
	CDebugger* debugger = new CDebugger(this, clientConnection);

	emit attachedDebugger(debugger);
}
