#include "sysinfodock.h"
#include <QResizeEvent>
#include <QtGui>

CSystemInfoDock::CSystemInfoDock(QWidget *parent)
	: QWidget(parent)
{
	QStandardItemModel* model;
	m_listWidget = new QTreeView(this);
	m_listWidget->setIndentation(10);
	m_listWidget->setRootIsDecorated(true);
	m_listWidget->setModel(model = new QStandardItemModel());
	m_listWidget->setEditTriggers(QTreeView::NoEditTriggers);
	m_listWidget->setAlternatingRowColors(true);
	model->invisibleRootItem()->setColumnCount(ColumnCount);
	model->setHeaderData(NameColumn,        Qt::Horizontal, tr("Name"));
	model->setHeaderData(DescriptionColumn, Qt::Horizontal, tr("Description"));
//	connect(m_listWidget, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedList(const QModelIndex &)));
	setEnable(false);
}

void CSystemInfoDock::resizeEvent(QResizeEvent* event)
{
	m_listWidget->resize(event->size());
}

void CSystemInfoDock::setEnable(bool enable)
{
	m_listWidget->setEnabled(enable);
}

void CSystemInfoDock::clear()
{
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(m_listWidget->model());
	QStandardItem* root = model->invisibleRootItem();
	while( root->rowCount() ) {
		root->removeRow(0);
	}
}

void CSystemInfoDock::update(const QString& valueName, const QString& description)
{
	QStandardItem* item = getItem(valueName);
	setItem(item, DescriptionColumn, description);
}

// 指定したパスのアイテムを取得
QStandardItem* CSystemInfoDock::getItem(const QString& valueName)
{
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(m_listWidget->model());
	QStandardItem* parent = model->invisibleRootItem();
	QStandardItem* item = NULL;
	QStringList key = valueName.split("/");
	QList<QStandardItem*> itemPath;

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
		parent = itemPath.back();
		// アイテム追加
		parent->setColumnCount(ColumnCount);
		parent->appendRow(item);
		// 下のレベルに移動
		itemPath.push_back(item);
		key.pop_front();
	}

	return item;
}

// アイテムの指定のカラムに値をセット
void CSystemInfoDock::setItem(QStandardItem* item, ColumnType type, const QString& value)
{
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(m_listWidget->model());
	model->setData(model->index(item->index().row(), int(type), item->index().parent()), value);
}

