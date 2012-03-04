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

// �\�����[�V�������擾
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
// �ǂݍ��݁E�ۑ�

bool CWorkSpaceModel::load(const QString & fileName)
{
	return false;
}

bool CWorkSpaceModel::save(const QString & fileName)
{
	return false;
}

//////////////////////////////////////////////////////////////////////

// �v���W�F�N�g�̒ǉ�
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

	projectItem->setPath(fileName);

	return projectItem;
}

// �t�@�C���̒ǉ�
CWorkSpaceItem* CWorkSpaceModel::appendFile(const QString & fileName, CWorkSpaceItem * parentItem)
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

		// �t�B���^�Ƀ}�b�`������̂̂ݓo�^
		foreach(const QString& pattern, tempProject->suffixFilter())
		{
			QRegExp re(pattern, Qt::CaseSensitive, QRegExp::Wildcard);
			if( re.exactMatch(fileName) )
			{
				// �v���W�F�N�g�c���[�Ƀt�@�C����ǉ�
				fileItem = appendProject(fileName);

				return fileItem;
			}
		}

		break; }
	case CWorkSpaceItem::Project: {
		// ��Ƀt�B���^�̓o�^������D�揇�ʂ����߂�
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
		// �t�B���^�Ƀ}�b�`������̂̂ݓo�^
		foreach(const QString& pattern, parentItem->suffixFilter())
		{
			QRegExp re(pattern, Qt::CaseSensitive, QRegExp::Wildcard);
			if( re.exactMatch(fileName) )
			{
				// �v���W�F�N�g�c���[�Ƀt�@�C����ǉ�

				fileItem = new CWorkSpaceItem(parentItem, CWorkSpaceItem::File, this);

				if( !insertRow(rowCount(parentItem->index()), fileItem, parentItem->index()) ) {
					delete fileItem;
					return NULL;
				}

				fileItem->setPath(fileName);

				return fileItem;
			}
		}
		break;
	default:
		return NULL;
	}

	return NULL;
}

// �폜
bool CWorkSpaceModel::remove(CWorkSpaceItem * item)
{
	return removeRow(item->parentPosition(), item->parent()->index());
}

bool CWorkSpaceModel::setAssignWidget(QWidget * widget)
{
	m_assignWidget = widget;
	return true;
}

QWidget* CWorkSpaceModel::assignWidget()
{
	return m_assignWidget;
}

bool CWorkSpaceModel::dataChanged(const QModelIndex & from, const QModelIndex & to)
{
	if( QTreeView *widget = dynamic_cast<QTreeView*>(m_assignWidget) ) {
		widget->dataChanged(from, to);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
// QAbstractItemModel �I�[�o�[���C�h

int CWorkSpaceModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}

bool CWorkSpaceModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	CWorkSpaceItem * item = getItem(index);

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
	case CWorkSpaceItem::PathRole:
		item->setPath(value.toString());
		return true;
	case CWorkSpaceItem::TypeRole:
		item->setType(CWorkSpaceItem::Type(value.toInt()));
		return true;
	case CWorkSpaceItem::NodeTypeRole:
		item->setNodeType(CWorkSpaceItem::NodeType(value.toInt()));
		return true;
	case CWorkSpaceItem::UuidRole:
		item->setUuid(value.toString());
		return true;
	case CWorkSpaceItem::SuffixFilterRole:
		item->setSuffixFilter(value.toStringList());
		return true;
	}

	return false;
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
	case CWorkSpaceItem::PathRole:
		return item->path();
	case CWorkSpaceItem::TypeRole:
		return item->type();
	case CWorkSpaceItem::NodeTypeRole:
		return item->nodeType();
	case CWorkSpaceItem::UuidRole:
		return item->uuid();
	case CWorkSpaceItem::SuffixFilterRole:
		return item->suffixFilter();
	}

	return QVariant();
}

QMap<int, QVariant> CWorkSpaceModel::itemData(const QModelIndex &index) const
{
	QMap<int, QVariant> roles = QAbstractItemModel::itemData(index);

	CWorkSpaceItem * item = getItem(index);

	if( item )
	{
		roles.insert(CWorkSpaceItem::PathRole,         item->path());
		roles.insert(CWorkSpaceItem::TypeRole,         item->type());
		roles.insert(CWorkSpaceItem::NodeTypeRole,     item->nodeType());
		roles.insert(CWorkSpaceItem::UuidRole,         item->uuid().toString());
		roles.insert(CWorkSpaceItem::SuffixFilterRole, item->suffixFilter());
	}
qDebug() << roles;

	return roles;
}

QModelIndex CWorkSpaceModel::index(int row, int column, const QModelIndex & parent) const
{
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

bool CWorkSpaceModel::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent)
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

