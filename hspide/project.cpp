#include <QtDebug>
#include <QFileInfo>
#include "project.h"
#include "projectitem.h"
#include "editor.h"
#include "projectreader.h"

/////////////////////////////////////////////////////////////////////

CProject::CProject(QObject *parent)
	: CProjectItem(parent)
{
}

CProject::~CProject()
{
}

// プロジェクトを読み込み
bool CProject::load(const QString & filename)
{
	CProjectReader reader;
	return reader.load(filename, *this);
}

// プロジェクトを保存
bool CProject::save(const QString & filename)
{
	return false;
}

// フォルダを取得
CProjectItem * CProject::getFolderItem(const QString & path, bool createAlways)
{
	CProjectItem *item = this;

	for(QStringListIterator ite(path.split("/"));
		ite.hasNext(); )
	{
		CFolderItem * folderItem = NULL;
		QString name = ite.next();

		for(int row = 0, rowNum = item->rowCount();
			row < rowNum; row++, folderItem = NULL)
		{
			if( NULL != (folderItem = dynamic_cast<CFolderItem*>(item->child(row))) &&
				!folderItem->fileName().compare(name, Qt::CaseSensitive) )
			{
				break;
			}
		}

		if( folderItem )
		{
			item = folderItem;
		}
		else if( createAlways )
		{
			folderItem = new CFolderItem(this);
			folderItem->setName(name);
			item->insertRow(rowCount(), folderItem);
			item = folderItem;
		}
		else
		{
			return NULL;
		}
	}

	return item;
}

// プロジェクトにファイルを追加
bool CProject::append(const QString & filename, const QString & path)
{
	CProjectItem *folder = getFolderItem(path);
	if( !folder ) {
		return false;
	}

	CProjectItem *item = new CProjectItem(this);
	item->setPath(filename);

	folder->insertRow(rowCount(), item);
	return true;
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

// プロジェクト名称取得
QString CProject::name() const
{
	return QFileInfo(filePath()).baseName();
}
