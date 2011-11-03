#include <QResizeEvent>
#include <QHeaderView>
#include <QStandardItem>
#include "projectdock.h"
#include "workspacemodel.h"

CProjectDock::CProjectDock(QWidget *parent)
	: QWidget(parent)
{
	mTree = new QTreeView(this);
	mTree->header()->hide();
	mTree->setRootIsDecorated(false);
//	mTree->setIndentation(12);
//	mTree->setUniformRowHeights(true);
	mTree->setEditTriggers(QTreeView::EditKeyPressed);

	connect(mTree, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedTree(const QModelIndex &)));
}

void CProjectDock::resizeEvent(QResizeEvent * event)
{
	mTree->resize(event->size());
}

// ソリューションを結合
bool CProjectDock::setWorkSpace(CWorkSpaceModel * workspace)
{
	mTree->setModel(workspace);
	mTree->expandAll();
	return true;
}

void CProjectDock::selectItem(CWorkSpaceItem * item)
{
	mTree->selectionModel()->clear();
//	mTree->selectionModel()->select(item->index(), QItemSelectionModel::Select);
}

void CProjectDock::doubleClickedTree(const QModelIndex & index)
{
////	QStandardItem *itemVoid = static_cast<QStandardItem*>(mTree->currentIndex().internalPointer());
//	QStandardItem *itemVoid = static_cast<QStandardItem*>(index.internalPointer());
//	               itemVoid = itemVoid->child(index.row(), index.column());
//
//	if( CFileItem *item = dynamic_cast<CFileItem*>(itemVoid) )
//	{
//		// シグナル発報
//		emit oepnProjectFileItem(item->filePath());
//	}
//	else if( CFolderItem *item = dynamic_cast<CFolderItem*>(itemVoid) )
//	{
//	}
}