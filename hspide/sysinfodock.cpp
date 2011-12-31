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
//	model->invisibleRootItem()->insertColumns(0, 2);
	model->invisibleRootItem()->setColumnCount(2);
	model->setHeaderData(0, Qt::Horizontal, tr("Name"));
	model->setHeaderData(1, Qt::Horizontal, tr("Description"));
//	connect(listWidget, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedList(const QModelIndex &)));
	int row = 0;
	QStandardItem *root = model->invisibleRootItem();
	QStandardItem *item, *item2;
	item = root;
	for (int i = 0; i < 4; ++i) {
		root->setColumnCount(2);
		root->appendRow(item = new QStandardItem(QString("item %0").arg(i)));
		model->setData(model->index(item->index().row(), 1, root->index()), QString(">>%0").arg(i));
	//	root->takeChild(item->index().row(), 1, root->index()), QString(">>%0").arg(i));
	//	QList<QStandardItem*> items;
	//	items << new QStandardItem(QString(">>%0").arg(i));
	//	root->appendColumn(items);
		root = item;
	}	
//    model->insertRow(row);
//	model->setData(model->index(row, 0), "a");
//	model->setData(model->index(row, 1), "b");
//	model->insertRow(1, model->index(row, 0));
//	model->setData(model->index(1, 0), "b");
//	model->setData(model->index(1, 1), "c");
}

void CSystemInfoDock::resizeEvent(QResizeEvent * event)
{
	listWidget->resize(event->size());
}

