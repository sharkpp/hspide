#include "projectdock.h"
#include <QResizeEvent>
#include <QTreeWidgetItem>

CProjectDock::CProjectDock(QWidget *parent)
	: QWidget(parent)
{
	mTree = new QTreeWidget(this);
	mTree->setColumnCount(1);
	QList<QTreeWidgetItem *> items;
	mTree->insertTopLevelItems(0, items);
	for (int i = 0; i < 10; ++i)
		items.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("item: %1").arg(i))));
	mTree->insertTopLevelItems(0, items);
}

void CProjectDock::resizeEvent(QResizeEvent * event)
{
	mTree->resize(event->size());
}

