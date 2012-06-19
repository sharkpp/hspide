#include <QDebug>
#include <QDir>
#include <QLocalSocket>
#include <QTemporaryFile>
#include "compiler.h"
#include "workspaceitem.h"
#include "documentpane.h"
#include "debugger.h"

CCompiler::CCompiler(QObject *parent)
	: QObject(parent)
	, m_uuid(QUuid::createUuid())
	, m_hspCompPath(QDir::toNativeSeparators("./"))
	, m_hspPath(QDir::toNativeSeparators("./"))
	, m_hspCommonPath(QDir::toNativeSeparators("./common/"))
	, m_targetItem(NULL)
	, m_compiler(NULL)
	, m_objTemp(NULL)
{
	updateCompilerPath();

	// 設定の変更通史の登録と設定からの初期化処理
	connect(&theConf, SIGNAL(updateConfiguration(const Configuration&)),
	        this,  SLOT(updateConfiguration(const Configuration&)));
	updateConfiguration(theConf);
}

const QUuid& CCompiler::uuid() const
{
	return m_uuid;
}

QTemporaryFile*	CCompiler::objTemp()
{
	return m_objTemp;
}

// シンボル一覧の取得
bool CCompiler::collectSymbols()
{
	QString program = m_hspCompPath + "hspcmp.exe";

	if( !QFileInfo(program).exists() )
	{
		return false;
	}

	QStringList arguments;
	arguments << "--symbol-put";

	QProcess* process = new QProcess(this);

	// 処理完了時の通知を登録
	connect(process, SIGNAL(finished(int,QProcess::ExitStatus)),
			this, SLOT(listedSymbolsFinished(int,QProcess::ExitStatus)));

	process->start(program, arguments);

	if( !process->waitForStarted() )
	{
		delete process;
		return false;
	}
	return true;
}

// シンボル一覧を取得
const CSymbolsList& CCompiler::symbols() const
{
	return m_highlightSymbols;
}

CWorkSpaceItem* CCompiler::compileItem()
{
	return m_targetItem;
}

// コンパイラ呼び出し
bool CCompiler::compile(CWorkSpaceItem* targetItem, bool debugMode)
{
	QString workDir = QDir::currentPath(); // いらないかも

	QString filename = targetItem->path();
	QUuid   uuid     = targetItem->uuid();

	m_targetItem = targetItem;
	m_objTemp = NULL;

	// シグナル発報
	emit compileStarted(uuid);

	switch( targetItem->type() )
	{
	case CWorkSpaceItem::Project:
		break;
	default:
		emit compileFailure();
		return false;
	}

	bool tempSave = false;

	QProcess* hspcmp = new QProcess(this);

	// 無題の場合は一時的なファイルに保存
	if( targetItem->isUntitled() )
	{
		CDocumentPane* document = targetItem->assignDocument();

		if( !document )
		{
			emit compileFailure();
			return false;
		}

		// 一時的なファイルに保存
		tempSave = true;
		QString baseName = QDir::toNativeSeparators(QDir(workDir).absoluteFilePath("hsptmp"));
		QTemporaryFile* tempFile = new QTemporaryFile(baseName, hspcmp);
		tempFile->setAutoRemove(true);
		if( !tempFile->open() )
		{
			emit compileFailure();
			return false;
		}
		filename = tempFile->fileName();
		document->save(filename);
	}

	QString program = m_hspCompPath + "hspcmp";
	QStringList arguments;
	arguments << "-C" << m_hspCommonPath
	          << "-H" << m_hspPath;
	if( debugMode ) { // デバッグモード
		arguments << "-d";
	}
	if( tempSave ) { // 一時的なファイルに保存
		QString baseName = QDir::toNativeSeparators(QDir(workDir).absoluteFilePath("obj"));
		m_objTemp = new QTemporaryFile(baseName, parent());
		m_objTemp->setAutoRemove(true);
		if( !m_objTemp->open() )
		{
		}
		arguments << "-o" << m_objTemp->fileName()
		          << "-r" << QString("?%1").arg(targetItem->uuid().toString());
	} else {
		arguments << "-r" << QString("?%1").arg(targetItem->uuid().toString());
	}
	arguments << filename;

	// 処理完了時の通知を登録
	connect(hspcmp, SIGNAL(error(QProcess::ProcessError)),      this, SLOT(hspcmpError(QProcess::ProcessError)));
	connect(hspcmp, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(hspcmpFinished(int,QProcess::ExitStatus)));
	connect(hspcmp, SIGNAL(readyReadStandardOutput()),          this, SLOT(hspcmpReadOutput()));

	m_compiler   = hspcmp;

	hspcmp->setWorkingDirectory(workDir);
	hspcmp->start(program, arguments);

	if( !hspcmp->waitForStarted() )
	{
		delete hspcmp;
		emit compileFailure();
		m_compiler   = NULL;
		return false;
	}

	return true;
}

void CCompiler::updateCompilerPath()
{
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

void CCompiler::hspcmpError(QProcess::ProcessError)
{
}

void CCompiler::hspcmpFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	QProcess* hspcmp = qobject_cast<QProcess*>(sender());

	// シグナル発報
	if( QProcess::NormalExit == exitStatus )
	{
		emit compileSuccessful();
	}
	else
	{
		emit compileFailure();
	}

	delete hspcmp;
}

void CCompiler::hspcmpReadOutput()
{
	QProcess* hspcmp = qobject_cast<QProcess*>(sender());
	QString   tmp(hspcmp->readAllStandardOutput());
	emit compileOutput(tmp);
}

// シンボルの取得完了
void CCompiler::listedSymbolsFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	QProcess* hspcmp = qobject_cast<QProcess*>(sender());
	QString tmp(hspcmp->readAllStandardOutput());

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
			QString& keyword = ite2.next();
			keyword = keyword.trimmed();
		}

		m_highlightSymbols.push_back(keywords);
	}

//	qDebug() << mSymbols;

	emit updatedSymbols();

	hspcmp->deleteLater();
}

