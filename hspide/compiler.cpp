#include <QDebug>
#include <QDir>
#include "compiler.h"
#include "solution.h"
#include "project.h"

CCompiler::CCompiler(QObject *parent)
	: QObject(parent)
	, mHspCompPath(QDir::toNativeSeparators("./"))
	, mHspPath(QDir::toNativeSeparators("./"))
	, mHspCommonPath(QDir::toNativeSeparators("./common/"))
	, mProcess(NULL)
{
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

// プロジェクトをビルド
void CCompiler::build(CProject * project)
{
	QString filename;
	if( !project->getMainSource(filename) )
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

// プロジェクトのビルド完了
void CCompiler::buildFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	// シグナル発報
	emit buildFinished(QProcess::NormalExit == exitStatus);
}

// ビルド中の出力を取得
void CCompiler::buildReadOutput()
{
	QString tmp(mProcess->readAllStandardOutput());
	emit buildOutput(tmp);
}
