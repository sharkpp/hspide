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
		solutionItem->setType(CWorkSpaceItem::Folder);
		solutionItem->setSubType(CWorkSpaceItem::Solution);
		solutionItem->setIcon(QIcon(":/images/tango/small/edit-copy.png"));
		solutionItem->setText(tr("(untitled)"));
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
	projectItem->setType(CWorkSpaceItem::File);
	projectItem->setSubType(CWorkSpaceItem::Project);
	projectItem->setIcon(QIcon(":/images/tango/small/folder.png"));
	projectItem->setText();

	if( !insertRows(0, 4, projectIndex) ) {
		return NULL;
	}

	CWorkSpaceItem * dependenceItem = getItem(index(0, 0, projectIndex));
	dependenceItem->setType(CWorkSpaceItem::Folder);
	dependenceItem->setSubType(CWorkSpaceItem::DependenceFolder);
	dependenceItem->setIcon(QIcon(":/images/tango/small/folder.png"));
	dependenceItem->setText(tr("Dependence"));

	CWorkSpaceItem * packItem = getItem(index(1, 0, projectIndex));
	packItem->setType(CWorkSpaceItem::Folder);
	packItem->setSubType(CWorkSpaceItem::PackFolder);
	packItem->setIcon(QIcon(":/images/tango/small/folder.png"));
	packItem->setText(tr("Packfile"));

	CWorkSpaceItem * sourceItem = getItem(index(2, 0, projectIndex));
	sourceItem->setType(CWorkSpaceItem::Folder);
	sourceItem->setSubType(CWorkSpaceItem::SourceFolder);
	sourceItem->setIcon(QIcon(":/images/tango/small/folder.png"));
	sourceItem->setText(tr("Source"));

	CWorkSpaceItem * resourceItem = getItem(index(3, 0, projectIndex));
	resourceItem->setType(CWorkSpaceItem::Folder);
	resourceItem->setSubType(CWorkSpaceItem::ResourceFolder);
	resourceItem->setIcon(QIcon(":/images/tango/small/folder.png"));
	resourceItem->setText(tr("Resource"));

	return projectItem;
}

// ファイルの追加
CWorkSpaceItem * CWorkSpaceModel::appendFile(const QString & fileName, CWorkSpaceItem * parentItem)
{
	if( !parentItem )
	{
		return NULL;
	}

	QModelIndex projectIndex = parentItem->index();
	QModelIndex itemIndex = index(0, 0, projectIndex);
	for(int row = 0, count = rowCount(projectIndex);
		row < count && CWorkSpaceItem::SourceFolder != getItem(itemIndex)->subType();
		row++, itemIndex = index(row, 0, projectIndex));

	if( CWorkSpaceItem::SourceFolder != getItem(itemIndex)->subType() ) {
		return NULL;
	}

	if( !insertRow(rowCount(itemIndex), itemIndex) ) {
		return NULL;
	}

	CWorkSpaceItem * fileItem
		= getItem(index(rowCount(itemIndex) - 1, 0, itemIndex));
	fileItem->setType(CWorkSpaceItem::File);
	fileItem->setSubType(CWorkSpaceItem::Default);
	fileItem->setIcon(QIcon(":/images/tango/small/text-x-generic.png"));
	fileItem->setPath(fileName);

	return fileItem;
}

// 削除
bool CWorkSpaceModel::remove(CWorkSpaceItem * item)
{
	return removeRow(item->parentPosition(), item->parent()->index());
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

	for(; count; --count, ++row) {
		item->insert(row, new CWorkSpaceItem(this));
	}

	endInsertRows();

	return true;
}

bool CWorkSpaceModel::removeRows(int row, int count, const QModelIndex & parent)
{
	CWorkSpaceItem * item = getItem(parent);

	beginRemoveRows(parent, row, row + count - 1);

	for(; count; --count) {
		item->remove(row);
	}

	endRemoveRows();

	return true;
}

