#include "varinfodock.h"
#include <QResizeEvent>
#include <QtGui>

CVariableInfoDock::CVariableInfoDock(QWidget *parent)
	: QWidget(parent)
	, m_keyBase(1)
{
	QStandardItemModel* model;
	m_listWidget = new QTreeView(this);
	m_listWidget->setIndentation(10);
	m_listWidget->setModel(model = new QStandardItemModel());
	m_listWidget->setEditTriggers(QTreeView::NoEditTriggers);
	m_listWidget->setAlternatingRowColors(true);
	model->invisibleRootItem()->setColumnCount(ColumnCount);
	model->setHeaderData(NameColumn,        Qt::Horizontal, tr("Name"));
	model->setHeaderData(TypeColumn,        Qt::Horizontal, tr("Type"));
	model->setHeaderData(DescriptionColumn, Qt::Horizontal, tr("Description"));
	connect(m_listWidget, SIGNAL(expanded(const QModelIndex &)), this, SLOT(onTreeExpanded(const QModelIndex &)));
	setEnable(false);
}

void CVariableInfoDock::resizeEvent(QResizeEvent* event)
{
	m_listWidget->resize(event->size());
}

void CVariableInfoDock::setEnable(bool enable)
{
	m_listWidget->setEnabled(enable);
}

void CVariableInfoDock::clear()
{
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(m_listWidget->model());
	QStandardItem* root = model->invisibleRootItem();
	while( root->rowCount() ) {
		root->removeRow(0);
	}
}

void CVariableInfoDock::update(const VARIABLE_INFO_TYPE& varInfo)
{
	QString path = toPath(varInfo);
	QStandardItem* item = getItem(path);
	setItem(item, TypeColumn,        varInfo.typeName);
	setItem(item, DescriptionColumn, varInfo.description);
	// 現在の階層の項目数を取得
	int lengthOf = -1;
	for(int i = 0; i < 4; i++) {
		if( 0 <= varInfo.index[i] ) {
			lengthOf = varInfo.length[i];
		}
	}
	if( lengthOf != item->rowCount() ) {
	//	while( item->rowCount() ) {
	//		item->removeRow(0);
	//	}
		// 子要素を追加
		//for(int i = 0; i < lengthOf; i++) {
		//	QString newPath = QString("%1/%2").arg(path).arg(i);
		//	QStandardItem* item_ = getItem(newPath);
		//	setItem(item_, DescriptionColumn, tr("loading..."));
		//}
	}
	m_varInfo[item->data().toLongLong()] = varInfo;
	// 添え字が(0,-1,-1,-1)の時だけ特別扱い
	if(  0 == varInfo.index[0] && 
		-1 == varInfo.index[1] && 
		-1 == varInfo.index[2] && 
		-1 == varInfo.index[3] )
	{
		VARIABLE_INFO_TYPE varInfo_ = varInfo;
		varInfo_.index[0] = -1;
		item = item->parent();
		setItem(item, TypeColumn,        varInfo.typeName);
		setItem(item, DescriptionColumn, varInfo.description);
		m_varInfo[item->data().toLongLong()] = varInfo_;
	}
}

// 指定したパスのアイテムを取得
QStandardItem* CVariableInfoDock::getItem(const QString& valueName)
{
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(m_listWidget->model());
	QStandardItem* parent = model->invisibleRootItem();
	QStandardItem* item = NULL;
	QStringList key = valueName.split("/");
	QList<QStandardItem*> itemPath;
	QStandardItem* rootItem = NULL;
	bool bCreateRoot = false;

	itemPath.push_back(parent);

	for(int row = 0; row < parent->rowCount(); row++)
	{
		item = parent->child(row);
		if( key.front() == item->text() )
		{
			itemPath.push_back(item);
			key.pop_front();
			// 全てたどれた？
			if( key.empty() ) {
				return item;
			}
			// 一つもぐる
			parent = item;
			row    = -1;
			continue;
		}
	}

	for(; !key.empty() ;)
	{
		item   = new QStandardItem(key.front());
		item->setData(m_keyBase++);
		parent = itemPath.back();
		//
		if( !rootItem && "@" == key.front() ) {
			bCreateRoot = true;
			rootItem = item;
		}
		// アイテム追加
		parent->setColumnCount(ColumnCount);
		parent->appendRow(item);
		// 下のレベルに移動
		itemPath.push_back(item);
		key.pop_front();
	}

	if( bCreateRoot ) {
		m_listWidget->expand(rootItem->index());
	}

	return item;
}

// アイテムの指定のカラムに値をセット
void CVariableInfoDock::setItem(QStandardItem* item, ColumnType type, const QString& value)
{
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(m_listWidget->model());
	model->setData(model->index(item->index().row(), int(type), item->index().parent()), value);
}

// 変数名からパスに変換
QString CVariableInfoDock::toPath(const VARIABLE_INFO_TYPE& varInfo)
{
	QRegExp re("([^@]+)(@(.*))?");
	QString path;

	if( 0 <= re.indexIn(varInfo.name) )
	{
		int index[4];
		QString varName = re.cap(1); // 変数名
		QString modName = re.cap(3); // モジュール名
		path = QString("%1/%2").arg(modName.isEmpty()?"@":modName).arg(varName);
		for(int i = 0; i < 4 && 0 <= varInfo.index[i]; i++) {
			path += QString("/%1").arg(varInfo.index[i]);
		}
	}
	return path;
}

void CVariableInfoDock::onTreeExpanded(const QModelIndex& index)
{
	QStandardItem* item = static_cast<QStandardItem*>(index.internalPointer());
	               item = item->child(index.row());
qlonglong x = item->data().toLongLong();
qDebug() << x << m_varInfo;

	QMap<qlonglong, VARIABLE_INFO_TYPE>::iterator
		ite = m_varInfo.find(item->data().toLongLong());
	if( ite != m_varInfo.end() )
	{
		VARIABLE_INFO_TYPE& info = *ite;
		QString path = toPath(info);

		// 現在の階層の項目数を取得
		int lengthOf = -1;
		int indexOf  = -1;
		for(int i = 0; i < 4; i++) {
			indexOf  = i;
			lengthOf = info.length[i];
			if( info.index[i] < 0 ) {
				break;
			}
		}

		for(int i = 0; i < lengthOf; i++) {
			int idx[4];
			for(int j = 0; j < 4; j++) {
				idx[j] = info.index[j];
			}
			idx[indexOf] = i;
			emit requestVariableInfo(info.name, idx);
			// 子要素を追加
			QString newPath = QString("%1/%2").arg(path).arg(i);
			QStandardItem* item_ = getItem(newPath);
			setItem(item_, DescriptionColumn, tr("loading..."));
			// 孫要素を追加
			if( indexOf + 1 < 4 &&
				0 < info.length[indexOf + 1] )
			{
				QString newPath2 = QString("%1/%2").arg(newPath).arg(i);
				item_ = getItem(newPath2);
				setItem(item_, DescriptionColumn, tr("loading..."));
			}
		}

		//QRegExp re("([^@]+)(@(.*))?");
		//if( 0 <= re.indexIn(info.name) )
		//{
		//	QString varName = re.cap(1); // 変数名
		//	QString modName = re.cap(3); // モジュール名

		//	varName = modName.isEmpty() ? "" : "@" + modName;

		//	emit requestVariableInfo(varName, index);
		//}
	}
}
