#include "messagedock.h"
#include <QResizeEvent>
#include <QtGui>

CMessageDock::CMessageDock(QWidget *parent)
	: QWidget(parent)
{
	QStandardItemModel* model;
	listWidget = new QTreeView(this);
	listWidget->setRootIsDecorated(false);
	listWidget->setSortingEnabled(true);
	listWidget->setModel(model = new QStandardItemModel());
	listWidget->setEditTriggers(QTreeView::NoEditTriggers);
	listWidget->setStyleSheet(
			"QTreeView {"
			"    show-decoration-selected: 1;"
			"}"
			"QTreeView::item {"
			"     border: 1px solid #d9d9d9;"
			"    border-top-color: transparent;"
			"    border-bottom-color: transparent;"
			"}"
			"QTreeView::item:hover {"
			"    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
			"    border: 1px solid #bfcde4;"
			"}"
			"QTreeView::item:selected {"
			"    border: 1px solid #567dbc;"
			"}"
			"QTreeView::item:selected:active{"
			"    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
			"}"
			"QTreeView::item:selected:!active {"
			"    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
			"}"
		);
	model->invisibleRootItem()->insertColumns(0, ColumnCount);
	model->setHeaderData(CategoryColumn,    Qt::Horizontal, tr("Category"));
	model->setHeaderData(DescriptionColumn, Qt::Horizontal, tr("Description"));
	model->setHeaderData(FileColumn,        Qt::Horizontal, tr("File"));
	model->setHeaderData(LineColumn,        Qt::Horizontal, tr("Line"));
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
	model->setData(model->index(row, CategoryColumn   ), "");
	model->setData(model->index(row, DescriptionColumn), description);
	model->setData(model->index(row, FileColumn       ), fileName);
	model->setData(model->index(row, LineColumn       ), QString("%1").arg(lineNo));
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
