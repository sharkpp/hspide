#include "project.h"
#include "editor.h"
#include <QDir>

CProject::CProject(const CProject::Config & config)
	: mConfig(config)
{
}

CProject::~CProject()
{
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
	QProcess *process = new QProcess(this);
	process->start(program, arguments);

	if( !process->waitForStarted() ) {
		emit buildFinished(false);
		return;
	}

	// 処理完了時の通知を登録
	connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(buildFinished(int,QProcess::ExitStatus)));

	// シグナル発報
	emit buildStart();
}

// プロジェクトのビルド完了
void CProject::buildFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	// シグナル発報
	emit buildFinished(QProcess::NormalExit == exitStatus);
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

