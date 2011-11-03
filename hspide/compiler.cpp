#include <QDebug>
#include <QDir>
#include "compiler.h"
#include "workspaceitem.h"

CCompiler::CCompiler(QObject *parent)
	: QObject(parent)
	, mHspCompPath(QDir::toNativeSeparators("./"))
	, mHspPath(QDir::toNativeSeparators("./"))
	, mHspCommonPath(QDir::toNativeSeparators("./common/"))
	, mProcess(NULL)
	, mProcessForListingSymbols(NULL)
{
	updateCompilerPath();
}

// コンパイラのパスを取得
const QString &  CCompiler::compilerPath() const
{
	return mHspCompPath;
}

// コンパイラのパスを指定
void CCompiler::setCompilerPath(const QString & path)
{
	mHspCompPath = QDir::toNativeSeparators(path);
	updateCompilerPath();
}

// HSPディレクトリのパスを取得
const QString &  CCompiler::hspPath() const
{
	return mHspPath;
}

// HSPディレクトリのパスを指定
void CCompiler::setHspPath(const QString & path)
{
	mHspPath = QDir::toNativeSeparators(path);
}

// HSPディレクトリのパスを取得
const QString &  CCompiler::hspCommonPath() const
{
	return mHspCommonPath;
}

// HSP commonディレクトリのパスを指定
void CCompiler::setHspCommonPath(const QString & path)
{
	mHspCommonPath = QDir::toNativeSeparators(path);
}

void CCompiler::updateCompilerPath()
{
	QString program = mHspCompPath + "hspcmp";
	QStringList arguments;
	arguments << "--symbol-put";

	delete mProcessForListingSymbols;
	mProcessForListingSymbols = new QProcess(this);

	mProcessForListingSymbols->start(program, arguments);

	if( !mProcessForListingSymbols->waitForStarted() ) {
		return;
	}

	// 処理完了時の通知を登録
	connect(mProcessForListingSymbols, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(listedSymbolsFinished(int,QProcess::ExitStatus)));
}

// シンボル一覧を取得
const QVector<QStringList> & CCompiler::symbols() const
{
	return mSymbols;
}

// プロジェクトをビルド
void CCompiler::build(CWorkSpaceItem * project)
{
	QString filename;
	//if( !project->getMainSource(filename) )
	{
		return;
	}

	QString program = mHspCompPath + "hspcmp";
	QStringList arguments;
	arguments << "-C" << mHspCommonPath
	          << "-H" << mHspPath
	          << filename;
	delete mProcess;
	mProcess = new QProcess(this);
	mProcess->start(program, arguments);

	if( !mProcess->waitForStarted() ) {
		emit buildFinished(false);
		delete mProcess;
		return;
	}

	// 処理完了時の通知を登録
	connect(mProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(buildFinished(int,QProcess::ExitStatus)));
	connect(mProcess, SIGNAL(readyReadStandardOutput()),          this, SLOT(buildReadOutput()));

	// シグナル発報
	emit buildStart();
}

// シンボルの取得完了
void CCompiler::listedSymbolsFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	QString tmp(mProcessForListingSymbols->readAllStandardOutput());

	mSymbols.clear();

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

		mSymbols.push_back(keywords);
	}

//	qDebug() << mSymbols;

	delete mProcessForListingSymbols;
}

// プロジェクトのビルド完了
void CCompiler::buildFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	// シグナル発報
	emit buildFinished(QProcess::NormalExit == exitStatus);
	delete mProcess;
}

// ビルド中の出力を取得
void CCompiler::buildReadOutput()
{
	QString tmp(mProcess->readAllStandardOutput());
	emit buildOutput(tmp);
}
