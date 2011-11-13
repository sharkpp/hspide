#include <QDebug>
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
	workspace->setAssignWidget(mTree);
	return true;
}

// アイテムを選択
void CProjectDock::selectItem(CWorkSpaceItem * item)
{
	mTree->selectionModel()->clear();
	mTree->selectionModel()->select(item->index(), QItemSelectionModel::Select|QItemSelectionModel::Current);
	// 選択したアイテムを見えるようにする
//	mTree->scrollTo(item->index());
}

// 現在のアイテムを取得
CWorkSpaceItem * CProjectDock::currentItem()
{
	QItemSelectionModel * model = mTree->selectionModel();

	if( !model->hasSelection() ) {
		return NULL;
	}

	QModelIndexList indexes = model->selectedIndexes();
	CWorkSpaceItem *item = static_cast<CWorkSpaceItem*>(indexes.front().internalPointer());

	return item;
}

// 現在のファイルを取得
CWorkSpaceItem * CProjectDock::currentFile()
{
	CWorkSpaceItem* item = currentItem();
	return CWorkSpaceItem::File != item->type()
	           ? item : NULL;;
}

// 現在のプロジェクトを取得
CWorkSpaceItem * CProjectDock::currentProject()
{
	CWorkSpaceItem* item = currentItem();

	for(; item && CWorkSpaceItem::Project != item->subType();
		item = item->parent());

	return item;
}

void CProjectDock::doubleClickedTree(const QModelIndex & index)
{
	CWorkSpaceItem *item = static_cast<CWorkSpaceItem*>(index.internalPointer());

	if( item )
	{
		// シグナル発報
		emit oepnItem(item);
	}
}
