#include <QDebug>
#include <QResizeEvent>
#include <QHeaderView>
#include <QStandardItem>
#include "workspacedock.h"
#include "workspacemodel.h"

class QTreeView_ : public QTreeView
{
public:
	QTreeView_(QWidget* parent) : QTreeView(parent) {}
	void dragEnterEvent(QDragEnterEvent *event) { event->acceptProposedAction(); }
	void dragMoveEvent(QDragMoveEvent *event)
	{
		event->acceptProposedAction();
		if( event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist") ) {
			event->setDropAction(Qt::MoveAction);
		}
	}
};

CWorkSpaceDock::CWorkSpaceDock(QWidget *parent)
	: QWidget(parent)
{
	treeWidget = new QTreeView_(this);
	treeWidget->header()->hide();
	treeWidget->setRootIsDecorated(true);
//	treeWidget->setIndentation(12);
//	treeWidget->setUniformRowHeights(true);
	treeWidget->setEditTriggers(QTreeView::EditKeyPressed);
	treeWidget->setExpandsOnDoubleClick(false);
	treeWidget->setDragEnabled(true);
	treeWidget->setAcceptDrops(true);
	treeWidget->setDropIndicatorShown(true);
//	setAcceptDrops(true);

	connect(treeWidget, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedTree(const QModelIndex &)));
	connect(treeWidget, SIGNAL(expanded(const QModelIndex &)),      this, SLOT(expandedTree(const QModelIndex &)));
}

void CWorkSpaceDock::resizeEvent(QResizeEvent* event)
{
	treeWidget->resize(event->size());
}

// �\�����[�V����������
bool CWorkSpaceDock::setWorkSpace(CWorkSpaceModel* workspace)
{
	treeWidget->setModel(workspace);
	treeWidget->expandAll();
	return true;
}

// �A�C�e����I��
void CWorkSpaceDock::selectItem(CWorkSpaceItem* item)
{
	treeWidget->selectionModel()->clear();
	treeWidget->selectionModel()->select(item->index(), QItemSelectionModel::Select|QItemSelectionModel::Current);
	// �I�������A�C�e����������悤�ɂ���
	treeWidget->scrollTo(item->index());
}

// ���݂̃A�C�e�����擾
CWorkSpaceItem* CWorkSpaceDock::currentItem()
{
	QItemSelectionModel* model = treeWidget->selectionModel();

	if( !model->hasSelection() ) {
		return NULL;
	}

	QModelIndexList indexes = model->selectedIndexes();
	if( indexes.isEmpty() ) {
		return NULL;
	}

	CWorkSpaceItem *item = static_cast<CWorkSpaceItem*>(indexes.front().internalPointer());

	return item;
}

// ���݂̃\�����[�V�������擾
CWorkSpaceItem* CWorkSpaceDock::currentSolution()
{
	CWorkSpaceModel* model = dynamic_cast<CWorkSpaceModel*>(treeWidget->model());

	if( !model ) {
		return NULL;
	}

	return model->solution();
}

// ���݂̃v���W�F�N�g���擾
CWorkSpaceItem* CWorkSpaceDock::currentProject()
{
	CWorkSpaceItem* item = currentItem();

	for(; item && CWorkSpaceItem::Project != item->type();
		item = item->parent());

	return item;
}

// �v���W�F�N�g���擾
QList<CWorkSpaceItem*> CWorkSpaceDock::projects()
{
	QList<CWorkSpaceItem*> result;

	CWorkSpaceModel* model = dynamic_cast<CWorkSpaceModel*>(treeWidget->model());
	CWorkSpaceItem*  solutionItem  = currentSolution();
	QModelIndex      solutionIndex = solutionItem->index();

	for(int row = 0, num = model->rowCount(solutionIndex);
		row < num; row++)
	{
		QModelIndex index = model->index(row, 0, solutionIndex);
		CWorkSpaceItem* item = static_cast<CWorkSpaceItem*>(index.internalPointer());
		result.append(item);
	}

	return result;
}

// ���݂̃t�@�C�����擾
CWorkSpaceItem* CWorkSpaceDock::currentFile()
{
	CWorkSpaceItem* item = currentItem();
	return CWorkSpaceItem::File != item->type()
	           ? item : NULL;;
}

void CWorkSpaceDock::doubleClickedTree(const QModelIndex& index)
{
	CWorkSpaceItem *item = static_cast<CWorkSpaceItem*>(index.internalPointer());

	//if( item &&
	//	!item->assignDocument() )
	//{
	//	treeWidget->setExpanded(index, false);
	//}

	if( item )
	{
		// �V�O�i������
		emit oepnItem(item);
	}
}

void CWorkSpaceDock::expandedTree(const QModelIndex& index)
{
	//CWorkSpaceItem *item = static_cast<CWorkSpaceItem*>(index.internalPointer());

	//if( item &&
	//	!item->assignDocument() )
	//{
	//	treeWidget->setExpanded(index, false);
	//}
}
