#include "project.h"
#include "editor.h"
#include <QDir>

CProject::CProject(const CProject::Config & config)
	: mConfig(config)
	, mProcess(NULL)
{
}

CProject::~CProject()
{
	delete mProcess;
}

// プロジェクトを読み込み
bool CProject::load()
{
	return false;
}

// プロジェクトを保存
bool CProject::save(const QString & filename)
{
	return false;
}

// プロジェクトにファイルを追加
bool CProject::append(const QString & filename)
{
	return false;
}

// プロジェクトから除外
bool CProject::remove(const QString & filename)
{
	return false;
}

// プロジェクトへエディタを関連付け
bool CProject::openFile(CEditor* editor)
{
//	editor->filename()

	return false;
}

// プロジェクト内のファイルを閉じる
bool CProject::closeFile(CEditor* editor)
{
	return false;
}

// プロジェクトをビルド
void CProject::build()
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
void CProject::buildFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	// シグナル発報
	emit buildFinished(QProcess::NormalExit == exitStatus);
}

// ビルド中の出力を取得
void CProject::buildReadOutput()
{
	QString tmp(mProcess->readAllStandardOutput());
	emit buildOutput(tmp);
}

// プロジェクト内のファイルを一時的に保存
bool CProject::saveTemp()
{
	return false;
}

// プロジェクト内の一次保存ファイルをクリア
bool CProject::clearTemp()
{
	return false;
}

