#include <QResizeEvent>
#include <QHeaderView>
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
	mTree->setIndentation(12);
}

void CProjectDock::resizeEvent(QResizeEvent * event)
{
	mTree->resize(event->size());
}

// ƒ\ƒŠƒ…[ƒVƒ‡ƒ“‚ðŒ‹‡
bool CProjectDock::setSolution(CSolution * solution)
{
	QStandardItemModel *model = new QStandardItemModel(this);
	model->invisibleRootItem()->appendRow(solution);
	mTree->setModel(model);
	mTree->expandAll();
	return true;
}
