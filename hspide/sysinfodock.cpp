#include "sysinfodock.h"
#include <QResizeEvent>
#include <QtGui>

CSystemInfoDock::CSystemInfoDock(QWidget *parent)
	: QWidget(parent)
{
	QStandardItemModel* model;
	listWidget = new QTreeView(this);
	listWidget->setRootIsDecorated(false);
	listWidget->setModel(model = new QStandardItemModel());
	listWidget->setEditTriggers(QTreeView::NoEditTriggers);
	model->invisibleRootItem()->setColumnCount(ColumnCount);
	model->setHeaderData(NameColumn,        Qt::Horizontal, tr("Name"));
	model->setHeaderData(DescriptionColumn, Qt::Horizontal, tr("Description"));
//	connect(listWidget, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedList(const QModelIndex &)));

	setItem(getItem("test/hoge"), DescriptionColumn, ">>hoge");
	setItem(getItem("test/fuga"), DescriptionColumn, ">>fuga");
	setItem(getItem("test"), DescriptionColumn, ">>test");
	setItem(getItem("fuga"), DescriptionColumn, ">>fuga");
}

void CSystemInfoDock::resizeEvent(QResizeEvent * event)
{
	listWidget->resize(event->size());
}

void CSystemInfoDock::setVariable(const QString & valueName, const QString & typeName, const QString & description)
{
}

// 指定したパスのアイテムを取得
QStandardItem* CSystemInfoDock::getItem(const QString & valueName)
{
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(listWidget->model());
	QStandardItem* parent = model->invisibleRootItem();
	QStandardItem* item;
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
void CSystemInfoDock::setItem(QStandardItem* item, ColumnType type, const QString & value)
{
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(listWidget->model());
	model->setData(model->index(item->index().row(), int(type), item->index().parent()), value);
}

