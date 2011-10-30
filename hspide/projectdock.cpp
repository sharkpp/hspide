#include <QResizeEvent>
#include <QHeaderView>
#include <QStandardItem>
#include "projectdock.h"
#include "solution.h"
#include "projectitem.h"

CProjectDock::CProjectDock(QWidget *parent)
	: QWidget(parent)
{
	mTree = new QTreeView(this);
	mTree->header()->hide();
	mTree->setRootIsDecorated(false);
//	mTree->setIndentation(12);
//	mTree->setUniformRowHeights(true);
	mTree->setEditTriggers(QTreeView::EditKeyPressed);

	mTree->setModel(new QStandardItemModel(this));

	connect(mTree, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedTree(const QModelIndex &)));
}

void CProjectDock::resizeEvent(QResizeEvent * event)
{
	mTree->resize(event->size());
}

// ソリューションを結合
bool CProjectDock::setSolution(CSolution * solution)
{
	QStandardItemModel *model = static_cast<QStandardItemModel*>(mTree->model());
	QStandardItem *     root  = model->invisibleRootItem();
	if( root->rowCount() ) {
		root->takeRow(0);
	}
	root->appendRow(solution);
	mTree->expandAll();
	return true;
}

void CProjectDock::selectItem(CProjectItem * item)
{
	mTree->selectionModel()->clear();
	mTree->selectionModel()->select(item->index(), QItemSelectionModel::Select);
}

void CProjectDock::doubleClickedTree(const QModelIndex & index)
{
//	QStandardItem *itemVoid = static_cast<QStandardItem*>(mTree->currentIndex().internalPointer());
	QStandardItem *itemVoid = static_cast<QStandardItem*>(index.internalPointer());
	               itemVoid = itemVoid->child(index.row(), index.column());

	if( CFileItem *item = dynamic_cast<CFileItem*>(itemVoid) )
	{
		// シグナル発報
		emit oepnProjectFileItem(item->filePath());
	}
	else if( CFolderItem *item = dynamic_cast<CFolderItem*>(itemVoid) )
	{
	}
}