#include <QFileInfo>
#include "projectitem.h"

CProjectItem::CProjectItem(QObject *parent)
{
}

CProjectItem::~CProjectItem()
{
}

//int CProjectItem::getRow(const CProjectItem & item) const
//{
//	int row = mChildren.size();
//	for(QVector<CProjectItem*>::iterator
//			ite = mChildren.begin(), last= mChildren.end();
//		ite != last; ++ite, ++row)
//	{
//		if( (*ite).data() == &item ) {
//			break;
//		}
//	}
//	if( mChildren.size() <= row ) {
//		return 0;
//	}
//	return row;
//}

//CProjectItem * CProjectItem::child(int row)
//{
//	if( mChildren.size() <= row ) {
//		return NULL;
//	}
//	return mChildren[row];
//}
//
//int CProjectItem::rowCount() const
//{
//	return mChildren.size();
//}
//
//bool CProjectItem::append(CProjectItem * item)
//{
//	mChildren.append(item);
//	item->setParent(this);
//	return true;
//}

bool CProjectItem::setPath(const QString & path)
{
	mItemPath = path;
	mItemName = QFileInfo(mItemPath).fileName();
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
