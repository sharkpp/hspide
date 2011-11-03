#include <QFileInfo>
#include "workspacemodel.h"

CWorkSpaceModel::CWorkSpaceModel(QObject * parent)
	: QAbstractItemModel(parent)
	, rootItem(new CWorkSpaceItem(this, this))
{
}

CWorkSpaceItem * CWorkSpaceModel::getItem(const QModelIndex & index) const
{
	void * ptr = index.isValid() ? index.internalPointer()
	                             : NULL;
	return
		ptr ? static_cast<CWorkSpaceItem*>(ptr)
		    : rootItem;
}

int CWorkSpaceModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}

QVariant CWorkSpaceModel::data(const QModelIndex & index, int role) const
{
	CWorkSpaceItem * item = getItem(index);

	if( !item ) {
		return QVariant();
	}

	switch(role)
	{
	case Qt::DisplayRole:
		return QString("test");
	case Qt::DecorationRole:
		return QIcon();
	case Qt::ToolTipRole:
		return QString("TTTT");
	}

	return QVariant();
}

QModelIndex CWorkSpaceModel::index(int row, int column, const QModelIndex & parent) const
{
//	if( parent.isValid() && parent.column() ) {
//		return QModelIndex();
//	}

	CWorkSpaceItem * item = getItem(parent);
	                 item = item->at(row);

	if( !item ) {
		return QModelIndex();
	}

	return createIndex(row, column, item);
}

QModelIndex CWorkSpaceModel::parent(const QModelIndex & index) const
{
	if( !index.isValid() ) {
		return QModelIndex();
	}

	CWorkSpaceItem * item   = getItem(index);
	CWorkSpaceItem * parent = item->parent();

	if( rootItem == parent ) {
		return QModelIndex();
	}

	return createIndex(parent->parentPosition(), 0, parent);
}

int CWorkSpaceModel::rowCount(const QModelIndex & parent) const
{
	CWorkSpaceItem * item = getItem(parent);
	return item ? item->count() : -1;
}

bool CWorkSpaceModel::insertRows(int row, int count, const QModelIndex & parent)
{
	CWorkSpaceItem * item = getItem(parent);

	beginInsertRows(parent, row, row + count - 1);

	for(; count; --count) {
		item->insert(row, new CWorkSpaceItem(this));
	}

	endInsertRows();

	return true;
}

