#include "breakpointdock.h"
#include <QResizeEvent>
#include <QtGui>

CBreakPointDock::CBreakPointDock(QWidget *parent)
	: QWidget(parent)
{
	QStandardItemModel* model;
	m_listWidget = new QTreeView(this);
	m_listWidget->setRootIsDecorated(false);
	m_listWidget->setIndentation(10);
	m_listWidget->setModel(model = new QStandardItemModel());
	m_listWidget->setEditTriggers(QTreeView::NoEditTriggers);
	model->invisibleRootItem()->setColumnCount(ColumnCount);
	model->setHeaderData(FileColumn,        Qt::Horizontal, tr("Name"));
	setEnable(false);
}

void CBreakPointDock::resizeEvent(QResizeEvent * event)
{
	m_listWidget->resize(event->size());
}

void CBreakPointDock::setEnable(bool enable)
{
	m_listWidget->setEnabled(enable);
}
