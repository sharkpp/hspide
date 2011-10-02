#include <QFileInfo>
#include "projectitem.h"

CProjectItem::CProjectItem(QObject *parent)
	: QObject(parent)
{
	setSizeHint(QSize(0, 16));
//	setEditable(false);
}

CProjectItem::~CProjectItem()
{
}

bool CProjectItem::setPath(const QString & path)
{
	mItemPath = path;
	mItemName = QFileInfo(mItemPath).fileName();
	setText(mItemName);
	return true;
}

bool CProjectItem::setName(const QString & name)
{
	mItemPath.clear();
	mItemName = name;
	setText(name);
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

//QVariant CProjectItem::data(int role ) const
//{
//	switch(role)
//	{
//	case Qt::DisplayRole:
//		return fileName();
//	default:
//		return QStandardItem::data(role);
//	}
//}
