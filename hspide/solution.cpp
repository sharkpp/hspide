#include "solution.h"
#include "project.h"
#include "editor.h"

CSolution::CSolution(QObject *parent, const CSolution::Config & config)
	: QObject(parent)
	, mConfig(config)
{
}

CSolution::~CSolution()
{
}

// ソリューションを読み込み
bool CSolution::load()
{
	return false;
}

// ソリューションを保存
bool CSolution::save(const QString & filename)
{
	return false;
}

// ソリューションにプロジェクトを追加
bool CSolution::append(const QString & filename)
{
	CProject::Config config;
//	config.compilerPath = ""

	QSharedPointer<CProject> newProject(new CProject(config));
	mProjects[filename] = newProject;

	// シグナルの通知先を登録
	connect(&*newProject, SIGNAL(buildStart()),        this, SLOT(buildStartProject()));
	connect(&*newProject, SIGNAL(buildFinished(bool)), this, SLOT(buildFinishedProject(bool)));

	return false;
}

// ソリューションからプロジェクトを除外
bool CSolution::remove(const QString & filename)
{
	QSharedPointer<CProject> oldProject = mProjects[filename];

	// シグナルの通知先の登録解除
//	disconnect(newProject, );

	mProjects.remove(filename);

	return false;
}

// ソリューションへエディタを関連付け
bool CSolution::openFile(CEditor* editor)
{
	for(QMap<QString, QSharedPointer<CProject> >::iterator
			ite = mProjects.begin(),
			last= mProjects.end();
		ite != last; ++ite)
	{
		(*ite)->openFile(editor);
	}
	return true;
}

// ソリューションからエディタを解除
bool CSolution::closeFile(CEditor* editor)
{
	for(QMap<QString, QSharedPointer<CProject> >::iterator
			ite = mProjects.begin(),
			last= mProjects.end();
		ite != last; ++ite)
	{
		(*ite)->closeFile(editor);
	}
	return true;
}

// ソリューションのビルド
void CSolution::build()
{
	for(QMap<QString, QSharedPointer<CProject> >::iterator
			ite = mProjects.begin(),
			last= mProjects.end();
		ite != last; ++ite)
	{
		(*ite)->build();
	}

	emit buildStart();
}

// プロジェクトのビルド開始
void CSolution::buildStartProject()
{
}

// プロジェクトのビルド完了
void CSolution::buildFinishedProject(bool successed)
{
	emit buildFinished(successed);
}

// ソリューションの実行
void CSolution::run()
{
}
