#include <QDir>
#include "compiler.h"
#include "solution.h"
#include "project.h"

CCompiler::CCompiler(QObject *parent)
	: QObject(parent)
	, mProcess(NULL)
{
}

// プロジェクトをビルド
void CCompiler::build()
{
#ifdef _DEBUG
	QDir::setCurrent(QDir::currentPath() + "\\debug\\");
#endif

	QString program = "./hspcmp";
	QStringList arguments;
	arguments << "-style" << "motif";
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
