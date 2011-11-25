#include <QDebug>
#include <QDir>
#include "compiler.h"
#include "workspaceitem.h"
#include "documentpane.h"

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

// コンパイラのパスを取得
const QString &  CCompiler::compilerPath() const
{
	return m_hspCompPath;
}

// コンパイラのパスを指定
void CCompiler::setCompilerPath(const QString & path)
{
	m_hspCompPath = QDir::toNativeSeparators(path);
	updateCompilerPath();
}

// HSPディレクトリのパスを取得
const QString &  CCompiler::hspPath() const
{
	return m_hspPath;
}

// HSPディレクトリのパスを指定
void CCompiler::setHspPath(const QString & path)
{
	m_hspPath = QDir::toNativeSeparators(path);
}

// HSPディレクトリのパスを取得
const QString &  CCompiler::hspCommonPath() const
{
	return m_hspCommonPath;
}

// HSP commonディレクトリのパスを指定
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

	// 処理完了時の通知を登録
	connect(m_listingSymbolsProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(listedSymbolsFinished(int,QProcess::ExitStatus)));
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

	QString program = m_hspCompPath + "hspcmp";
	QStringList arguments;
	arguments << "-C" << m_hspCommonPath
	          << "-H" << m_hspPath;
	if( debugMode ) { // デバッグモード
		arguments << "-d";
	}
	if( tempSave ) { // 一時的なファイルに保存
		arguments << "-o" << "obj"
		          << "-r" << "???";
	}
	if( buildAfterRun ) { // ビルド後に実行
		arguments << "-e";
	}
	arguments << filename;
	delete m_compilerProcess;
	m_compilerProcess = new QProcess(this);
	m_compilerProcess->setWorkingDirectory(workDir);
	m_compilerProcess->start(program, arguments);

	if( !m_compilerProcess->waitForStarted() ) {
		emit buildFinished(false);
		delete m_compilerProcess;
		m_compilerProcess = NULL;
		return false;
	}

	// 処理完了時の通知を登録
	connect(m_compilerProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(buildFinished(int,QProcess::ExitStatus)));
	connect(m_compilerProcess, SIGNAL(readyReadStandardOutput()),          this, SLOT(buildReadOutput()));

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
	QString tmp(m_listingSymbolsProcess->readAllStandardOutput());

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

	delete m_listingSymbolsProcess;
	m_listingSymbolsProcess = NULL;
}

// プロジェクトのビルド完了
void CCompiler::buildFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	// シグナル発報
	emit buildFinished(QProcess::NormalExit == exitStatus);
	delete m_compilerProcess;
	m_compilerProcess = NULL;
}

// ビルド中の出力を取得
void CCompiler::buildReadOutput()
{
	QString tmp(m_compilerProcess->readAllStandardOutput());
	emit buildOutput(tmp);
}
