#include "messagedock.h"
#include <QResizeEvent>
#include <QtGui>

CMessageDock::CMessageDock(QWidget *parent)
	: QWidget(parent)
{
	QStandardItemModel* model;
	listWidget = new QTreeView(this);
	listWidget->setRootIsDecorated(false);
	listWidget->setModel(model = new QStandardItemModel());
	listWidget->setEditTriggers(QTreeView::NoEditTriggers);
	model->invisibleRootItem()->insertColumns(0, 4);
	model->setHeaderData(MsgCategory,    Qt::Horizontal, tr("Category"));
	model->setHeaderData(MsgDescription, Qt::Horizontal, tr("Description"));
	model->setHeaderData(MsgFile,        Qt::Horizontal, tr("File"));
	model->setHeaderData(MsgLine,        Qt::Horizontal, tr("Line"));
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
	m_messages.clear();
}

void CMessageDock::addMessage(const QUuid & uuid, const QString & fileName, int lineNo, const QString & description)
{
	QStandardItemModel* model
		= static_cast<QStandardItemModel*>(listWidget->model());
	int row = model->rowCount();
    model->insertRow(row);
    model->setData(model->index(row, MsgCategory   ), "");
    model->setData(model->index(row, MsgDescription), description);
    model->setData(model->index(row, MsgFile       ), fileName);
    model->setData(model->index(row, MsgLine       ), QString("%1").arg(lineNo));
	//
	MessageInfoType info;
	info.description= description;
	info.uuid		= uuid;
	info.fileName	= fileName;
	info.lineNo		= lineNo;
	m_messages.push_back(info);
}

void CMessageDock::doubleClickedList(const QModelIndex & index)
{
//	QStandardItem* item = static_cast<QStandardItem*>(index.internalPointer());
	MessageInfoType & info = m_messages[index.row()];
	emit gotoLine(info.uuid, info.lineNo);
}
