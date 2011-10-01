#include <QFileInfo>
#include "projectitem.h"

CProjectItem::CProjectItem(QObject *parent)
	: QObject(parent)
{
}

CProjectItem::~CProjectItem()
{
}

bool CProjectItem::setPath(const QString & path)
{
	mItemPath = path;
	mItemName = QFileInfo(mItemPath).fileName();
	return true;
}

bool CProjectItem::setName(const QString & name)
{
	mItemPath.clear();
	mItemName = name;
	return true;
}

const QString & CProjectItem::filePath() const
{
	return mItemPath;
}

const QString & CProjectItem::fileName() const
{
	return mItemName;
}

QVariant CProjectItem::data(int role ) const
{
	switch(role)
	{
	case Qt::DisplayRole:
		return fileName();
	default:
		return QStandardItem::data(role);
	}
}
