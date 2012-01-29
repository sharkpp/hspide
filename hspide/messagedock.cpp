#include "messagedock.h"
#include <QResizeEvent>
#include <QtGui>

CMessageDock::CMessageDock(QWidget *parent)
	: QWidget(parent)
{
	QStandardItemModel* model;
	m_listWidget = new QTreeView(this);
	m_listWidget->setRootIsDecorated(false);
	m_listWidget->setSortingEnabled(true);
	m_listWidget->setModel(model = new QStandardItemModel());
	m_listWidget->setEditTriggers(QTreeView::NoEditTriggers);
	m_listWidget->setIndentation(0);
	m_listWidget->setStyleSheet(
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
	model->setHeaderData(FileNameColumn,    Qt::Horizontal, tr("File"));
	model->setHeaderData(LineNoColumn,      Qt::Horizontal, tr("Line"));
	model->setHeaderData(DescriptionColumn, Qt::Horizontal, tr("Description"));
	m_listWidget->setColumnWidth(CategoryColumn, 16 + 10/*‰½‚Ì’l‚¾‚ë‚¤H*/);
	m_listWidget->setColumnWidth(FileNameColumn, m_listWidget->fontMetrics().width(QLatin1Char('9')) * 20);
	m_listWidget->setColumnWidth(LineNoColumn,   m_listWidget->fontMetrics().width(QLatin1Char('9')) * 5);
//	m_listWidget->header()->setResizeMode(DescriptionColumn, QHeaderView::Stretch);
//	m_listWidget->header()->setResizeMode(FileNameColumn,    QHeaderView::ResizeToContents);
//	m_listWidget->header()->setResizeMode(LineNoColumn,      QHeaderView::ResizeToContents);
//	m_listWidget->header()->setResizeMode(FileNameColumn,    QHeaderView::Fixed);
//	m_listWidget->header()->setResizeMode(LineNoColumn,      QHeaderView::Fixed);
	connect(m_listWidget, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedList(const QModelIndex &)));
}

void CMessageDock::resizeEvent(QResizeEvent * event)
{
	m_listWidget->resize(event->size());
}

void CMessageDock::clear()
{
	QStandardItemModel* model
		= static_cast<QStandardItemModel*>(m_listWidget->model());
	if( model->rowCount() ) {
		model->removeRows(0, model->rowCount());
	}
	m_messages.clear();
}

void CMessageDock::addMessage(const QUuid & uuid, const QString & fileName, int lineNo, CategoryType category, const QString & description)
{
	QString iconPath, categoryName;
	switch(category)
	{
	case InfomationCategory:
		iconPath     = ":images/tango/small/dialog-information.png";
		categoryName = tr("Information");
		break;
	case WarningCategory:
		iconPath     = ":images/tango/small/dialog-warning.png";
		categoryName = tr("Warning");
		break;
	case ErrorCategory:
		iconPath     = ":images/tango/small/dialog-error.png";
		categoryName = tr("Error");
		break;
	default:
		break;
	}

	QStandardItemModel* model
		= static_cast<QStandardItemModel*>(m_listWidget->model());
	int row = model->rowCount();
	model->insertRow(row);
	if( !iconPath.isEmpty() ) {
		model->setData(model->index(row, CategoryColumn   ), QIcon(iconPath), Qt::DecorationRole);
		model->setData(model->index(row, CategoryColumn   ), categoryName);
	}
	model->setData(model->index(row, DescriptionColumn), description);
	model->setData(model->index(row, FileNameColumn   ), fileName);
	model->setData(model->index(row, LineNoColumn     ), 0 < lineNo ? QString("%1").arg(lineNo) : "");
	model->setData(model->index(row, LineNoColumn     ), Qt::AlignRight, Qt::TextAlignmentRole);
	//
	MessageInfoType info;
	info.description= description;
	info.category	= category;
	info.uuid		= uuid;
	info.fileName	= fileName;
	info.lineNo		= lineNo;
	m_messages.push_back(info);
}

void CMessageDock::doubleClickedList(const QModelIndex & index)
{
//	QStandardItem* item = static_cast<QStandardItem*>(index.internalPointer());
	MessageInfoType & info = m_messages[index.row()];
	if( 0 < info.lineNo ) {
		emit gotoLine(info.uuid, info.lineNo);
	}
}
