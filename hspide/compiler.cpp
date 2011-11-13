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

// プロジェクトをビルド
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

	// 処理完了時の通知を登録
	connect(m_compilerProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(buildFinished(int,QProcess::ExitStatus)));
	connect(m_compilerProcess, SIGNAL(readyReadStandardOutput()),          this, SLOT(buildReadOutput()));

	// シグナル発報
	emit buildStart();
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
}

// プロジェクトのビルド完了
void CCompiler::buildFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	// シグナル発報
	emit buildFinished(QProcess::NormalExit == exitStatus);
	delete m_compilerProcess;
}

// ビルド中の出力を取得
void CCompiler::buildReadOutput()
{
	QString tmp(m_compilerProcess->readAllStandardOutput());
	emit buildOutput(tmp);
}
