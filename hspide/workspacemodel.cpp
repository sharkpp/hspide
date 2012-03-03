#include <QtGui>
#include "workspacemodel.h"

CWorkSpaceModel::CWorkSpaceModel(QObject * parent)
	: QAbstractItemModel(parent)
	, m_assignWidget(NULL)
	, rootItem(new CWorkSpaceItem(this, this))
{
	QModelIndex rootIndex = createIndex(rootItem->parentPosition(), 0, rootItem);
	CWorkSpaceItem* item  = new CWorkSpaceItem(this, CWorkSpaceItem::Solution, this);
	if( insertRow(0, item, rootIndex) ) {
	} else {
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

// ソリューションを取得
CWorkSpaceItem* CWorkSpaceModel::solution()
{
	return rootItem->at(0);
}

bool CWorkSpaceModel::insertRow(int row, CWorkSpaceItem * item, const QModelIndex & parent)
{
	QVector<CWorkSpaceItem*> items;
	items.push_back(item);
	return insertRows(row, items, parent);
}

bool CWorkSpaceModel::insertRows(int row, const QVector<CWorkSpaceItem*> & items, const QModelIndex & parent)
{
	CWorkSpaceItem * parentItem = getItem(parent);

	beginInsertRows(parent, row, row + items.count() - 1);

	foreach(CWorkSpaceItem* item, items) {
		parentItem->insert(row++, item);
	}

	endInsertRows();

	return true;
}

bool CWorkSpaceModel::appendRow(CWorkSpaceItem * item, const QModelIndex & parent)
{
	return insertRow(rowCount(parent), item, parent);
}

bool CWorkSpaceModel::appendRows(const QVector<CWorkSpaceItem*> & items, const QModelIndex & parent)
{
	return insertRows(rowCount(parent), items, parent);
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
	CWorkSpaceItem*projectItem= new CWorkSpaceItem(this, CWorkSpaceItem::Project, this);

	int row = rowCount(solutionIndex);
	if( !insertRow(row, projectItem, solutionIndex) ) {
		return NULL;
	}

	QModelIndex projectIndex = index(row, 0, solutionIndex);
	QVector<CWorkSpaceItem*> items;
	items.push_back(new CWorkSpaceItem(projectItem, CWorkSpaceItem::DependenceFolder, this));
	items.push_back(new CWorkSpaceItem(projectItem, CWorkSpaceItem::PackFolder,       this));
	items.push_back(new CWorkSpaceItem(projectItem, CWorkSpaceItem::SourceFolder,     this));
	items.push_back(new CWorkSpaceItem(projectItem, CWorkSpaceItem::ResourceFolder,   this));

	if( !insertRows(0, items, projectIndex) ) {
		return NULL;
	}

	items.clear();

	return projectItem;
}

// ファイルの追加
CWorkSpaceItem * CWorkSpaceModel::appendFile(const QString & fileName, CWorkSpaceItem * parentItem)
{
	if( !parentItem )
	{
		return NULL;
	}

	QModelIndex projectIndex  = parentItem->index();
	QModelIndex itemIndex     = index(0, 0, projectIndex);
	for(int row = 0, count = rowCount(projectIndex);
		row < count && CWorkSpaceItem::SourceFolder != getItem(itemIndex)->type();
		row++, itemIndex = index(row, 0, projectIndex));

	if( CWorkSpaceItem::SourceFolder != getItem(itemIndex)->type() ) {
		return NULL;
	}

	CWorkSpaceItem* fileItem
		= new CWorkSpaceItem(parentItem, CWorkSpaceItem::File, this);

	if( !insertRow(rowCount(itemIndex), fileItem, itemIndex) ) {
		return NULL;
	}

	fileItem->setPath(fileName);

	return fileItem;
}

// 削除
bool CWorkSpaceModel::remove(CWorkSpaceItem * item)
{
	return removeRow(item->parentPosition(), item->parent()->index());
}

bool CWorkSpaceModel::setAssignWidget(QWidget * widget)
{
	m_assignWidget = widget;
	return true;
}

//////////////////////////////////////////////////////////////////////
// QAbstractItemModel オーバーライド

int CWorkSpaceModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}

bool CWorkSpaceModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if( QTreeView *widget = dynamic_cast<QTreeView*>(m_assignWidget) ) {
		widget->dataChanged(index, index);
	}
	return true;
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
		return item->path();
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

Qt::ItemFlags CWorkSpaceModel::flags(const QModelIndex & index) const
{
	Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

	if( index.isValid() ) {
		return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
	} else {
		return Qt::ItemIsDropEnabled | defaultFlags;
	}
}

bool CWorkSpaceModel::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
	if( action == Qt::IgnoreAction )
	{
		return true;
	}

	CWorkSpaceItem* parentItem = getItem(parent);
	if( !parentItem )
	{
		return false;
	}

qDebug()<< __FUNCTION__<<data->formats();
	if( !data->hasUrls() ||
		data->urls().empty() )
	{
		return false;
	}


	QList<QUrl> urls = data->urls();
	for(int i = 0; i < urls.size(); i++)
	{
		QString filePath = urls.at(i).toLocalFile();
		appendFile(filePath, parentItem);
	}

	return true;
}

Qt::DropActions CWorkSpaceModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

bool CWorkSpaceModel::insertRows(int row, int count, const QModelIndex & parent)
{
	CWorkSpaceItem * parentItem = getItem(parent);

	beginInsertRows(parent, row, row + count - 1);

	for(; count; --count, ++row) {
		parentItem->insert(row, new CWorkSpaceItem(this));
	}

	endInsertRows();

	return true;
}

bool CWorkSpaceModel::removeRows(int row, int count, const QModelIndex & parent)
{
	CWorkSpaceItem * parentItem = getItem(parent);

	beginRemoveRows(parent, row, row + count - 1);

	for(; count; --count) {
		parentItem->remove(row);
	}

	endRemoveRows();

	return true;
}

