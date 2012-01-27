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
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addWidget(m_toolBar = new QToolBar(this));
	layout->addWidget(m_listWidget = new QTreeView(this));
	layout->addWidget(m_treeWidget = new QTreeView(this));

	QStandardItemModel* model;
	m_listWidget->setRootIsDecorated(false);
	m_listWidget->setSortingEnabled(true);
	m_listWidget->setAlternatingRowColors(true);
	m_listWidget->setModel(model = new QStandardItemModel());
	m_listWidget->setEditTriggers(QTreeView::NoEditTriggers);
	model->invisibleRootItem()->setColumnCount(ColumnCount);
	model->setHeaderData(LineNoColumn,      Qt::Horizontal, tr("LineNo"));
	model->setHeaderData(TypeColumn,        Qt::Horizontal, tr("Type"));
	model->setHeaderData(DescriptionColumn, Qt::Horizontal, tr("Description"));
	m_listWidget->setColumnWidth(LineNoColumn,   m_listWidget->fontMetrics().width(QLatin1Char('9')) * 5);
	m_listWidget->setColumnWidth(TypeColumn,     m_listWidget->fontMetrics().width(QLatin1Char('9')) * 8);
	connect(m_listWidget, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedList(const QModelIndex &)));

	m_treeWidget->setVisible(false);

	m_toolBar->setStyleSheet("QToolBar{border:none}");
	m_toolBar->setIconSize(QSize(16, 16));
	QAction * tabListAct = m_toolBar->addAction(QIcon(":/images/tango/small/document-new.png"), tr("Tab list"));
	connect(tabListAct,  SIGNAL(triggered()), this, SLOT(onTabList()));
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
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(m_listWidget->model());
	QStandardItem* root = model->invisibleRootItem();
	while( root->rowCount() ) {
		root->removeRow(0);
	}
	m_symbolInfo.clear();
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
		= static_cast<QStandardItemModel*>(m_listWidget->model());
	int row = model->rowCount();
	model->insertRow(row);
	model->setData(model->index(row, LineNoColumn),      m_symbolInfo.size(), Qt::UserRole + 1);
	model->setData(model->index(row, LineNoColumn),      QString("%1").arg(lineNo));
	model->setData(model->index(row, TypeColumn),        typeName);
	model->setData(model->index(row, DescriptionColumn), description);

	SymbolInfoType info;
	info.uuid		= uuid;
	info.fileName	= fileName;
	info.lineNo		= lineNo;
	info.type		= type;
	info.description = description;
	m_symbolInfo.push_back(info);

	return true;
}

void CSymbolDock::doubleClickedList(const QModelIndex & index)
{
	QStandardItem* item = static_cast<QStandardItem*>(index.internalPointer());
	int indexOfInfo = item->child(index.row(), LineNoColumn)->data(Qt::UserRole + 1).toInt();
	SymbolInfoType& info = m_symbolInfo[indexOfInfo];
	emit gotoLine(info.uuid, info.lineNo);
}

void CSymbolDock::onTabList()
{
	bool mode = m_treeWidget->isVisible();
	m_treeWidget->setVisible(!mode);
	m_listWidget->setVisible( mode);
}
