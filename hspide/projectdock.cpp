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
	//mTree->setColumnCount(1);
	//QList<QTreeWidgetItem *> items;
	//mTree->insertTopLevelItems(0, items);
	//for (int i = 0; i < 10; ++i)
	//	items.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("item: %1").arg(i))));
	//mTree->insertTopLevelItems(0, items);
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
bool CProjectDock::setSolution(CSolution * solution)
{
	QStandardItemModel *model = new QStandardItemModel(this);
	model->invisibleRootItem()->appendRow(solution);
	mTree->setModel(model);
	mTree->expandAll();
	return true;
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