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
}

void CMessageDock::resizeEvent(QResizeEvent * event)
{
	listWidget->resize(event->size());
}

