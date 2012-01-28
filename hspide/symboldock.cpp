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

	QStandardItemModel* model;
	m_listWidget->setRootIsDecorated(false);
	m_listWidget->setSortingEnabled(true);
	m_listWidget->setAlternatingRowColors(true);
	m_listWidget->setModel(model = new QStandardItemModel());
	m_listWidget->setEditTriggers(QTreeView::NoEditTriggers);
	m_listWidget->setIndentation(10);
	model->invisibleRootItem()->setColumnCount(ColumnCount);
	model->setHeaderData(NameColumn,     Qt::Horizontal, tr("Name"));
	model->setHeaderData(TypeColumn,     Qt::Horizontal, tr("Type"));
	model->setHeaderData(LineNoColumn,   Qt::Horizontal, tr("LineNo"));
	model->setHeaderData(FileNameColumn, Qt::Horizontal, tr("FileName"));
	m_listWidget->setColumnWidth(NameColumn,   m_listWidget->fontMetrics().width(QLatin1Char('9')) * 8);
	m_listWidget->setColumnWidth(TypeColumn,   m_listWidget->fontMetrics().width(QLatin1Char('9')) * 8);
	m_listWidget->setColumnWidth(LineNoColumn, m_listWidget->fontMetrics().width(QLatin1Char('9')) * 5);
	connect(m_listWidget, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onTreeDoubleClicked(const QModelIndex &)));
	connect(m_listWidget, SIGNAL(collapsed(const QModelIndex &)), m_listWidget, SLOT(expand(const QModelIndex &)));

	m_toolBar->setStyleSheet("QToolBar{border:none}");
	m_toolBar->setIconSize(QSize(16, 16));
	QAction * tabListAct = m_toolBar->addAction(QIcon(":/images/tango/small/document-new.png"), tr("onTest"));
	connect(tabListAct,  SIGNAL(triggered()), this, SLOT(onTest()));
}

bool CSymbolDock::analyze(CDocumentPane* document)
{
	CWorkSpaceItem* item = document->assignItem();
	QString text = document->editor()->toPlainText();

	text.replace(QChar('\n'), QChar::LineSeparator);

	clear();

	Hsp3Lexer lexer;
	lexer.reset(text);

	append(item->uuid(), item->path(), 1, "", "@", TypeModule);

	int  lineNo = -1;
	bool prevColon = false;

	while( lexer.scan() )
	{
		if( Hsp3Lexer::TypeLabel == lexer.type() &&
			(lineNo != lexer.lineNo() || prevColon))
		{
			append(item->uuid(), item->path(), lexer.lineNo(), lexer.text(), "@", TypeLabel);
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

bool CSymbolDock::append(const QUuid& uuid, const QString& fileName, int lineNo, const QString& name, const QString& scope, SymbolType type)
{
	QString typeName;
	switch(type) {
	case TypeModule:       typeName = tr("Module"); break;
	case TypeUserSub:      typeName = tr("Sub"); break;
	case TypeUserFunction: typeName = tr("Func"); break;
	case TypeLabel:        typeName = tr("Label"); break;
	default:;
	}

	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(m_listWidget->model());
	QStandardItem* rootItem   = model->invisibleRootItem();
	QStandardItem* scopeItem  = NULL;

	// スコープ(モジュール名)を検索
	for(int row = 0; row < rootItem->rowCount(); row++, scopeItem = NULL)
	{
		scopeItem = rootItem->child(row);
		if( scope == scopeItem->text() )
		{
			break;
		}
	}

	// スコープが見つからなかったので追加
	if( !scopeItem )
	{
		scopeItem = new QStandardItem(scope);
		rootItem->setColumnCount(ColumnCount);
		rootItem->appendRow(scopeItem);
		model->setData(model->index(scopeItem->index().row(), RefIndexColumn, scopeItem->index().parent()), -1, Qt::UserRole + 1);
	}

	QStandardItem* item;

	if( TypeModule == type )
	{
		item = scopeItem;
	}
	else
	{
		// 指定スコープに追加
		item = new QStandardItem(name);
		scopeItem->setColumnCount(ColumnCount);
		scopeItem->appendRow(item);
	}

	model->setData(model->index(item->index().row(), RefIndexColumn, item->index().parent()), m_symbolInfo.size(), Qt::UserRole + 1);
	model->setData(model->index(item->index().row(), NameColumn,     item->index().parent()), TypeModule != type ? name : scope);
	model->setData(model->index(item->index().row(), TypeColumn,     item->index().parent()), typeName);
	model->setData(model->index(item->index().row(), LineNoColumn,   item->index().parent()), QString("%1").arg(lineNo));
	model->setData(model->index(item->index().row(), FileNameColumn, item->index().parent()), fileName);

	SymbolInfoType info;
	info.uuid		= uuid;
	info.fileName	= fileName;
	info.lineNo		= lineNo;
	info.name		= name;
	info.scope		= scope;
	info.type		= type;
	m_symbolInfo.push_back(info);

	m_listWidget->expand(scopeItem->index());

	return true;
}

void CSymbolDock::onTreeDoubleClicked(const QModelIndex & index)
{
	QStandardItem* item = static_cast<QStandardItem*>(index.internalPointer());
	int indexOfInfo = item->child(index.row(), RefIndexColumn)->data(Qt::UserRole + 1).toInt();
	if( 0 <= indexOfInfo ) {
		SymbolInfoType& info = m_symbolInfo[indexOfInfo];
		emit gotoLine(info.uuid, info.lineNo);
	}
}

void CSymbolDock::onTest()
{
}
