#include <QObject>
#include <QAbstractItemModel>
#include "workspaceitem.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CWorkSpaceModel
	: public QAbstractItemModel
{
	Q_OBJECT

	CWorkSpaceItem*	rootItem;

public:

	CWorkSpaceModel(QObject * parent = NULL);
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex & index ) const;
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

	virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

private:

	CWorkSpaceItem * getItem(const QModelIndex & index) const;

};
