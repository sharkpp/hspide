#include "symboldock.h"
#include "documentpane.h"
#include "workspaceitem.h"
#include <QDebug>
#include "hsp3lexer.h"
#include <QResizeEvent>
#include <QtGui>

CSymbolDock::CSymbolDock(QWidget *parent)
	: QWidget(parent)
{
	QStandardItemModel* model;
	listWidget = new QTreeView(this);
	listWidget->setRootIsDecorated(false);
	listWidget->setSortingEnabled(true);
	listWidget->setAlternatingRowColors(true);
	listWidget->setModel(model = new QStandardItemModel());
	listWidget->setEditTriggers(QTreeView::NoEditTriggers);
	model->invisibleRootItem()->setColumnCount(ColumnCount);
	model->setHeaderData(FileNameColumn,    Qt::Horizontal, tr("FileName"));
	model->setHeaderData(LineNoColumn,      Qt::Horizontal, tr("LineNo"));
	model->setHeaderData(TypeColumn,        Qt::Horizontal, tr("Type"));
	model->setHeaderData(DescriptionColumn, Qt::Horizontal, tr("Description"));
	listWidget->setColumnWidth(FileNameColumn, listWidget->fontMetrics().width(QLatin1Char('9')) * 16);
	listWidget->setColumnWidth(LineNoColumn,   listWidget->fontMetrics().width(QLatin1Char('9')) * 4);
	listWidget->setColumnWidth(TypeColumn,     listWidget->fontMetrics().width(QLatin1Char('9')) * 8);
	connect(listWidget, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedList(const QModelIndex &)));
}

void CSymbolDock::resizeEvent(QResizeEvent * event)
{
	listWidget->resize(event->size());
}

bool CSymbolDock::analyze(CDocumentPane* document)
{
	CWorkSpaceItem* item = document->assignItem();
	QString text = document->editor()->toPlainText();

	text.replace(QChar('\n'), QChar::LineSeparator);

	clear();

	Hsp3Lexer lexer;
	lexer.reset(text);

	int  lineNo = -1;
	bool prevColon = false;

	while( lexer.scan() )
	{
		if( Hsp3Lexer::TypeLabel == lexer.type() &&
			(lineNo != lexer.lineNo() || prevColon))
		{
			append(item->uuid(), item->path(), lexer.lineNo(), TypeLabel, lexer.text());
		}
		prevColon = ":" == lexer.text();
		lineNo    = lexer.lineNo();
	}

	return true;
}

bool CSymbolDock::clear()
{
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(listWidget->model());
	QStandardItem* root = model->invisibleRootItem();
	while( root->rowCount() ) {
		root->removeRow(0);
	}
	return true;
}

bool CSymbolDock::append(const QUuid& uuid, const QString& fileName, int lineNo, SymbolType type, const QString& description)
{
	QString typeName;
	switch(type) {
	case TypeUserSub:      typeName = tr("Sub"); break;
	case TypeUserFunction: typeName = tr("Func"); break;
	case TypeLabel:        typeName = tr("Label"); break;
	default:;
	}

	QStandardItemModel* model
		= static_cast<QStandardItemModel*>(listWidget->model());
	int row = model->rowCount();
	model->insertRow(row);
	model->setData(model->index(row, FileNameColumn),    uuid.toString(), Qt::UserRole + 1);
	model->setData(model->index(row, FileNameColumn),    fileName);
	model->setData(model->index(row, LineNoColumn),      QString("%1").arg(lineNo));
	model->setData(model->index(row, TypeColumn),        typeName);
	model->setData(model->index(row, DescriptionColumn), description);

	return true;
}

void CSymbolDock::doubleClickedList(const QModelIndex & index)
{
	QStandardItem* item = static_cast<QStandardItem*>(index.internalPointer());
//	QString fileName(item->child(index.row(), FileNameColumn)->text());
	QUuid   uuid(    item->child(index.row(), FileNameColumn)->data(Qt::UserRole + 1).toString());
	int     lineNo(  item->child(index.row(), LineNoColumn  )->text().toInt());
	emit gotoLine(uuid, lineNo);
}
