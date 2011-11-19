#include <QDebug>
#include <QResizeEvent>
#include <QHeaderView>
#include <QStandardItem>
#include "projectdock.h"
#include "workspacemodel.h"

CProjectDock::CProjectDock(QWidget *parent)
	: QWidget(parent)
{
	treeWidget = new QTreeView(this);
	treeWidget->header()->hide();
	treeWidget->setRootIsDecorated(false);
//	treeWidget->setIndentation(12);
//	treeWidget->setUniformRowHeights(true);
	treeWidget->setEditTriggers(QTreeView::EditKeyPressed);

	connect(treeWidget, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedTree(const QModelIndex &)));
}

void CProjectDock::resizeEvent(QResizeEvent * event)
{
	treeWidget->resize(event->size());
}

// �\�����[�V����������
bool CProjectDock::setWorkSpace(CWorkSpaceModel * workspace)
{
	treeWidget->setModel(workspace);
	treeWidget->expandAll();
	workspace->setAssignWidget(treeWidget);
	return true;
}

// �A�C�e����I��
void CProjectDock::selectItem(CWorkSpaceItem * item)
{
	treeWidget->selectionModel()->clear();
	treeWidget->selectionModel()->select(item->index(), QItemSelectionModel::Select|QItemSelectionModel::Current);
	// �I�������A�C�e����������悤�ɂ���
//	treeWidget->scrollTo(item->index());
}

// ���݂̃A�C�e�����擾
CWorkSpaceItem * CProjectDock::currentItem()
{
	QItemSelectionModel * model = treeWidget->selectionModel();

	if( !model->hasSelection() ) {
		return NULL;
	}

	QModelIndexList indexes = model->selectedIndexes();
	CWorkSpaceItem *item = static_cast<CWorkSpaceItem*>(indexes.front().internalPointer());

	return item;
}

// ���݂̃t�@�C�����擾
CWorkSpaceItem * CProjectDock::currentFile()
{
	CWorkSpaceItem* item = currentItem();
	return CWorkSpaceItem::File != item->type()
	           ? item : NULL;;
}

// ���݂̃v���W�F�N�g���擾
CWorkSpaceItem * CProjectDock::currentProject()
{
	CWorkSpaceItem* item = currentItem();

	for(; item && CWorkSpaceItem::Project != item->type();
		item = item->parent());

	return item;
}

void CProjectDock::doubleClickedTree(const QModelIndex & index)
{
	CWorkSpaceItem *item = static_cast<CWorkSpaceItem*>(index.internalPointer());

	if( item )
	{
		// �V�O�i������
		emit oepnItem(item);
	}
}
