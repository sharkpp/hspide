#include <QCryptographicHash>
#include <QtDebug>
#include "solution.h"
#include "project.h"
#include "editor.h"
#include "solutionreader.h"

/////////////////////////////////////////////////////////////////////

CSolution::CSolution(QObject *parent)
	: CProjectItem(parent)
{
}

CSolution::~CSolution()
{
}

// ソリューションを読み込み
bool CSolution::load(const QString & filename)
{
	CSolutionReader reader;
	return reader.load(filename, *this);
}

// ソリューションを保存
bool CSolution::save(const QString & filename)
{
	return false;
}

// ソリューションにプロジェクトを追加
CProject * CSolution::append(const QString & filename)
{
	CProject * newProject = new CProject(this);

	newProject->setPath(filename);

	if( !filename.isEmpty() ) {
		newProject->load(filename);
	}

	appendRow(newProject);

	return newProject;
}

// ソリューションからプロジェクトを除外
bool CSolution::remove(const QString & filename)
{
//	QSharedPointer<CProject> oldProject = mProjects[filename];
//	mProjects.remove(filename);
	return false;
}

// ソリューションへエディタを関連付け
bool CSolution::openFile(CEditor* editor)
{
	for(int row = 0, rowNum = rowCount();
		row < rowNum; row++)
	{
		if( CProject * project = dynamic_cast<CProject*>(child(row)) )
		{
			project->openFile(editor);
		}
	}
	return true;
}

// ソリューションからエディタを解除
bool CSolution::closeFile(CEditor* editor)
{
	for(int row = 0, rowNum = rowCount();
		row < rowNum; row++)
	{
		if( CProject * project = dynamic_cast<CProject*>(child(row)) )
		{
			project->closeFile(editor);
		}
	}
	return true;
}

// ソリューション名を取得
QString CSolution::title() const
{
	return QString();
}

// ソリューション名を取得
QString CSolution::filename() const
{
	return QString();
}

// プロジェクト数を取得
int CSolution::count() const
{
	return rowCount();
}

// プロジェクトを取得
CProject & CSolution::at(int index) const
{
	return *dynamic_cast<CProject*>(child(index));
}

