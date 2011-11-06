#include <QFileInfo>
#include "workspacemodel.h"

CWorkSpaceModel::CWorkSpaceModel(QObject * parent)
	: QAbstractItemModel(parent)
	, rootItem(new CWorkSpaceItem(this, this))
{
	QModelIndex rootIndex = createIndex(rootItem->parentPosition(), 0, rootItem);
	if( insertRow(0, rootIndex) ) {
		QModelIndex solutionIndex = index(0, 0, rootIndex);
		CWorkSpaceItem * solutionItem = getItem(solutionIndex);
		solutionItem->setIcon(QIcon(":/images/tango/small/edit-copy.png"));
		solutionItem->setText(tr("(no title)"));
	//	solutionItem->
	}
}

CWorkSpaceItem * CWorkSpaceModel::getItem(const QModelIndex & index) const
{
	void * ptr = index.isValid() ? index.internalPointer()
	                             : NULL;
	return
		ptr ? static_cast<CWorkSpaceItem*>(ptr)
		    : rootItem;
}

//////////////////////////////////////////////////////////////////////
// 読み込み・保存

bool CWorkSpaceModel::load(const QString & fileName)
{
	return false;
}

bool CWorkSpaceModel::save(const QString & fileName)
{
	return false;
}

//////////////////////////////////////////////////////////////////////

// プロジェクトの追加
CWorkSpaceItem * CWorkSpaceModel::appendProject(const QString & fileName)
{
	QModelIndex rootIndex     = createIndex(rootItem->parentPosition(), 0, rootItem);
	QModelIndex solutionIndex = index(0, 0, rootIndex);

	int row = rowCount(solutionIndex);
	if( !insertRow(row, solutionIndex) ) {
		return NULL;
	}

	QModelIndex projectIndex = index(row, 0, solutionIndex);
	CWorkSpaceItem * projectItem = getItem(projectIndex);
	projectItem->setIcon(QIcon(":/images/tango/small/folder.png"));
	projectItem->setText(tr("(no title)"));

	if( !insertRows(0, 4, projectIndex) ) {
		return NULL;
	}

	CWorkSpaceItem * DependenceItem = getItem(index(0, 0, projectIndex));
	DependenceItem->setIcon(QIcon(":/images/tango/small/folder.png"));
	DependenceItem->setText(tr("Dependence"));

	CWorkSpaceItem * packItem = getItem(index(1, 0, projectIndex));
	packItem->setIcon(QIcon(":/images/tango/small/folder.png"));
	packItem->setText(tr("Packfile"));

	CWorkSpaceItem * sourceItem = getItem(index(2, 0, projectIndex));
	sourceItem->setIcon(QIcon(":/images/tango/small/folder.png"));
	sourceItem->setText(tr("Source"));

	CWorkSpaceItem * resourceItem = getItem(index(3, 0, projectIndex));
	resourceItem->setIcon(QIcon(":/images/tango/small/folder.png"));
	resourceItem->setText(tr("Resource"));

	return projectItem;
}

//////////////////////////////////////////////////////////////////////
// QAbstractItemModel オーバーライド

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
		return item->text();
	case Qt::DecorationRole:
		return item->icon();
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

