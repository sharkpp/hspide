#include "messagedock.h"
#include <QResizeEvent>
#include <QtGui>

CMessageDock::CMessageDock(QWidget *parent)
	: QWidget(parent)
{
	QStandardItemModel* model;
	listWidget = new QTreeView(this);
	listWidget->setRootIsDecorated(false);
//	listWidget->header()->inser
	listWidget->setModel(model = new QStandardItemModel());
	model->invisibleRootItem()->insertColumns(0, 4);
	model->setHeaderData(0, Qt::Horizontal, tr("Category"));
	model->setHeaderData(1, Qt::Horizontal, tr("Description"));
	model->setHeaderData(2, Qt::Horizontal, tr("File"));
	model->setHeaderData(3, Qt::Horizontal, tr("Line"));
	connect(listWidget, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedList(const QModelIndex &)));
}

void CMessageDock::resizeEvent(QResizeEvent * event)
{
	listWidget->resize(event->size());
}

void CMessageDock::clear()
{
	QStandardItemModel* model
		= static_cast<QStandardItemModel*>(listWidget->model());
	if( model->rowCount() ) {
		model->removeRows(0, model->rowCount());
	}
}

void CMessageDock::addMessage(const QString & fileName, int lineNum, const QString & description)
{
	QStandardItemModel* model
		= static_cast<QStandardItemModel*>(listWidget->model());
	int row = model->rowCount();
    model->insertRow(row);
    model->setData(model->index(row, 0), "");
    model->setData(model->index(row, 1), description);
    model->setData(model->index(row, 2), fileName);
    model->setData(model->index(row, 3), QString("%1").arg(lineNum));
}

void CMessageDock::doubleClickedList(const QModelIndex & index)
{
	QStandardItem *item = static_cast<QStandardItem*>(index.internalPointer());
}
