#include <QtGui>
#include "workspacemodel.h"
#include "global.h"

CWorkSpaceModel::CWorkSpaceModel(QObject* parent)
	: QAbstractItemModel(parent)
	, rootItem(new CWorkSpaceItem(this, this))
{
	//// 空のプロジェクトを追加
	//QModelIndex rootIndex = createIndex(rootItem->parentPosition(), 0, rootItem);
	//CWorkSpaceItem* item  = new CWorkSpaceItem(this, CWorkSpaceItem::Solution, this);
	//if( insertRow(0, item, rootIndex) ) {
	//	item->setUuid(theFile->assign(""));
	//} else {
	//}
}

CWorkSpaceItem* CWorkSpaceModel::getItem(const QModelIndex& index) const
{
	void* ptr = index.isValid() ? index.internalPointer()
	                             : NULL;
	return
		ptr ? static_cast<CWorkSpaceItem*>(ptr)
		    : rootItem;
}

// ソリューションを取得
CWorkSpaceItem* CWorkSpaceModel::solution()
{
	return
		rootItem->count() && CWorkSpaceItem::Solution == rootItem->at(0)->type()
			? rootItem->at(0)
			: rootItem;
}

bool CWorkSpaceModel::insertRow(int row, CWorkSpaceItem* item, const QModelIndex& parent)
{
	return insertRows(row, QVector<CWorkSpaceItem*>() << item, parent);
}

bool CWorkSpaceModel::insertRows(int row, const QVector<CWorkSpaceItem*>& items, const QModelIndex& parent)
{
	bool result = false;
	CWorkSpaceItem* parentItem = getItem(parent);

	beginInsertRows(parent, row, row + items.count() - 1);

	foreach(CWorkSpaceItem* item, items) {
		result |= parentItem->insert(row++, item);
		connect(item, SIGNAL(loadComplete()), this, SLOT(onLoadComplete()));
		connect(item, SIGNAL(saveComplete()), this, SLOT(onSaveComplete()));
	}

	endInsertRows();

	return result;
}

bool CWorkSpaceModel::appendRow(CWorkSpaceItem* item, const QModelIndex& parent)
{
	return insertRow(rowCount(parent), item, parent);
}

bool CWorkSpaceModel::appendRows(const QVector<CWorkSpaceItem*>& items, const QModelIndex& parent)
{
	return insertRows(rowCount(parent), items, parent);
}

//////////////////////////////////////////////////////////////////////
// 読み込み・保存

bool CWorkSpaceModel::load(const QString& fileName)
{
	return false;
}

bool CWorkSpaceModel::save(const QString& fileName)
{
	return false;
}

//////////////////////////////////////////////////////////////////////

// ソリューションが存在するか
bool CWorkSpaceModel::isSolutionExist() const
{
	return rootItem->count() && CWorkSpaceItem::Solution == rootItem->at(0)->type();
}

// ソリューションを追加
CWorkSpaceItem* CWorkSpaceModel::insertSolution()
{
	if( isSolutionExist() )
	{
		return rootItem->at(0);
	}

	CWorkSpaceItem* tempSplution = new CWorkSpaceItem(this, CWorkSpaceItem::Solution, this);
	// 親子関係を挿げ替える
	QList<CWorkSpaceItem*> children = rootItem->take();
	for(int i = 0; i < children.size(); i++) {
		tempSplution->insert(i, children.at(i));
		rootItem    ->insert(i, new CWorkSpaceItem(this, CWorkSpaceItem::Project, this));
	}
	removeRows(0, children.size(), rootItem->index());
	insertRow(0, tempSplution, rootItem->index());
	return tempSplution;
}

// プロジェクトの追加
CWorkSpaceItem* CWorkSpaceModel::appendProject(const QString& fileName)
{
	QModelIndex solutionIndex = rootItem == solution() ? QModelIndex() : solution()->index();
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

	projectItem->setUuid(theFile->assign(fileName));

	// 通知
	emit loadComplete(projectItem);

	return projectItem;
}

// ファイルの追加
CWorkSpaceItem* CWorkSpaceModel::appendFile(const QString& fileName, CWorkSpaceItem* parentItem)
{
	CWorkSpaceItem* fileItem = NULL;

	if( !parentItem )
	{
		return NULL;
	}

	switch( parentItem->type() )
	{
	case CWorkSpaceItem::File:
		for(CWorkSpaceItem* item = parentItem->parent();
			item && CWorkSpaceItem::UnkownType != item->type();
			item = item->parent())
		{
			if( NULL != (fileItem = appendFile(fileName, item)) )
			{
				return fileItem;
			}
		}
		break;
	case CWorkSpaceItem::Solution: {
		QSharedPointer<CWorkSpaceItem> tempProject(new CWorkSpaceItem(NULL, CWorkSpaceItem::Project));

		// フィルタにマッチするもののみ登録
		foreach(const QString& pattern, tempProject->suffixFilter())
		{
			QRegExp re(pattern, Qt::CaseSensitive, QRegExp::Wildcard);
			if( re.exactMatch(fileName) )
			{
				// プロジェクトツリーにファイルを追加
				fileItem = appendProject(fileName);

				return fileItem;
			}
		}

		break; }
	case CWorkSpaceItem::Project: {
		// 先にフィルタの登録数から優先順位を決める
		QMultiMap<int, CWorkSpaceItem*> searchOrder;
		QModelIndex itemIndex = index(0, 0, parentItem->index());
		for(int row = 0, count = rowCount(parentItem->index());
			row < count; row++, itemIndex = index(row, 0, parentItem->index()))
		{
			CWorkSpaceItem* item = getItem(itemIndex);
			searchOrder.insert(-item->suffixFilter().size(), item);
		}
		for(QMultiMap<int, CWorkSpaceItem*>::iterator i = searchOrder.begin();
			i != searchOrder.end(); ++i)
		{
			if( NULL != (fileItem = appendFile(fileName, i.value())) )
			{
				return fileItem;
			}
		}
		break; }
//	case CWorkSpaceItem::DependenceFolder:
	case CWorkSpaceItem::PackFolder:
	case CWorkSpaceItem::SourceFolder:
	case CWorkSpaceItem::ResourceFolder:
		// フィルタにマッチするもののみ登録
		foreach(const QString& pattern, parentItem->suffixFilter())
		{
			QRegExp re(pattern, Qt::CaseSensitive, QRegExp::Wildcard);
			if( re.exactMatch(fileName) )
			{
				// プロジェクトツリーにファイルを追加

				fileItem = new CWorkSpaceItem(parentItem, CWorkSpaceItem::File, this);

				if( !insertRow(rowCount(parentItem->index()), fileItem, parentItem->index()) ) {
					delete fileItem;
					return NULL;
				}

				fileItem->setUuid(theFile->assign(fileName));
				fileItem->setText(theFile->fileName(fileItem->uuid()));

				return fileItem;
			}
		}
		break;
	default:
		return NULL;
	}

	return NULL;
}

// 削除
bool CWorkSpaceModel::remove(CWorkSpaceItem* item)
{
	return removeRow(item->parentPosition(), item->parent()->index());
}

//////////////////////////////////////////////////////////////////////

// 
void CWorkSpaceModel::onLoadComplete()
{
	CWorkSpaceItem* item = qobject_cast<CWorkSpaceItem*>(sender());
	emit loadComplete(item);
}

// 
void CWorkSpaceModel::onSaveComplete()
{
	CWorkSpaceItem* item = qobject_cast<CWorkSpaceItem*>(sender());
	emit saveComplete(item);
}

//////////////////////////////////////////////////////////////////////
// QAbstractItemModel オーバーライド

int CWorkSpaceModel::columnCount(const QModelIndex& /*parent*/) const
{
	return 1;
}

bool CWorkSpaceModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	CWorkSpaceItem* item = getItem(index);

	if( !item ) {
		return false;
	}

	switch(role)
	{
	case Qt::DisplayRole:
		item->setText(value.toString());
		return true;
	case Qt::DecorationRole:
		item->setIcon(value.value<QIcon>());
		return true;
	case Qt::ToolTipRole:
		return true;
	case CWorkSpaceItem::UuidRole:
		item->setUuid(value.toString());
		return true;
	case CWorkSpaceItem::TypeRole:
		item->setType(CWorkSpaceItem::Type(value.toInt()));
		return true;
	case CWorkSpaceItem::NodeTypeRole:
		item->setNodeType(CWorkSpaceItem::NodeType(value.toInt()));
		return true;
	case CWorkSpaceItem::SuffixFilterRole:
		item->setSuffixFilter(value.toStringList());
		return true;
	}

	emit dataChanged(index, index);

	return true;
}

QVariant CWorkSpaceModel::data(const QModelIndex& index, int role) const
{
	CWorkSpaceItem* item = getItem(index);

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
		return theFile->path(item->uuid());
	case CWorkSpaceItem::UuidRole:
        return item->uuid().toString();
	case CWorkSpaceItem::TypeRole:
		return item->type();
	case CWorkSpaceItem::NodeTypeRole:
		return item->nodeType();
	case CWorkSpaceItem::SuffixFilterRole:
		return item->suffixFilter();
	}

	return QVariant();
}

QMap<int, QVariant> CWorkSpaceModel::itemData(const QModelIndex &index) const
{
	if( !index.isValid() ) {
		return QMap<int, QVariant>();
	}

	QMap<int, QVariant> roles = QAbstractItemModel::itemData(index);

	CWorkSpaceItem* item = getItem(index);

	if( item )
	{
		roles.insert(CWorkSpaceItem::UuidRole,         item->uuid().toString());
		roles.insert(CWorkSpaceItem::TypeRole,         item->type());
		roles.insert(CWorkSpaceItem::NodeTypeRole,     item->nodeType());
		roles.insert(CWorkSpaceItem::SuffixFilterRole, item->suffixFilter());
	}

	return roles;
}

QModelIndex CWorkSpaceModel::index(int row, int column, const QModelIndex& parent) const
{
	CWorkSpaceItem* item = getItem(parent);
	                item = item->at(row);

	if( !item ) {
		return QModelIndex();
	}

	return createIndex(row, column, item);
}

QModelIndex CWorkSpaceModel::parent(const QModelIndex& index) const
{
	if( !index.isValid() ) {
		return QModelIndex();
	}

	CWorkSpaceItem* item   = getItem(index);
	CWorkSpaceItem* parent = item->parent();

	if( !parent || rootItem == parent ) {
		return QModelIndex();
	}

	return createIndex(parent->parentPosition(), 0, parent);
}

int CWorkSpaceModel::rowCount(const QModelIndex& parent) const
{
	CWorkSpaceItem* item = getItem(parent);
	return item->count();
}

Qt::ItemFlags CWorkSpaceModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

	if( index.isValid() ) {
		CWorkSpaceItem* item = getItem(index);
		if( item &&
			CWorkSpaceItem::FileNode == item->nodeType() &&
			CWorkSpaceItem::Project  != item->type() )
		{
			return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
		}
		return Qt::ItemIsDropEnabled | defaultFlags;
	} else {
		return Qt::ItemIsDropEnabled | defaultFlags;
	}
}

bool CWorkSpaceModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
	if( action == Qt::IgnoreAction )
	{
		return true;
	}

	CWorkSpaceItem* parentItem = getItem(parent);
	if( !parentItem ||
		CWorkSpaceItem::DependenceFolder == parentItem->type() )
	{
		return false;
	}

qDebug()<< __FUNCTION__<<data->formats();

	if( data->hasUrls() )
	{
		if( data->urls().empty() )
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
	else if( data->hasFormat("application/x-qabstractitemmodeldatalist") )
	{
		QByteArray encoded = data->data("application/x-qabstractitemmodeldatalist");
		QDataStream stream(&encoded, QIODevice::ReadOnly);

		while( !stream.atEnd() )
		{
			int row, col;
			QMap<int,  QVariant> roleDataMap;
			stream >> row >> col >> roleDataMap;
			qDebug() << row << col << roleDataMap;
		}

		return QAbstractItemModel::dropMimeData(data, action, row, column, parent);
	}

	return false;
}

Qt::DropActions CWorkSpaceModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

bool CWorkSpaceModel::insertRows(int row, int count, const QModelIndex& parent)
{
	CWorkSpaceItem* parentItem = getItem(parent);
	bool result = false;

	beginInsertRows(parent, row, row + count - 1);

	for(; count; --count, ++row) {
		result |= parentItem->insert(row, new CWorkSpaceItem(this));
	}

	endInsertRows();

	return result;
}

bool CWorkSpaceModel::removeRows(int row, int count, const QModelIndex& parent)
{
	CWorkSpaceItem* parentItem = getItem(parent);

	if( !count )
	{
		return false;
	}

	beginRemoveRows(parent, row, row + count - 1);

	for(; count; --count) {
		parentItem->remove(row);
	}

	endRemoveRows();

	return true;
}

