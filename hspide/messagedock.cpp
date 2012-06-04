#include <QtGui>
#include "messagedock.h"
#include "global.h"

CMessageDock::CMessageDock(QWidget *parent)
	: QWidget(parent)
	, m_countOfErrorMessages(0)
	, m_countOfWarningMessages(0)
	, m_countOfInfomationMessages(0)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addWidget(m_toolBar = new QToolBar(this));
	layout->addWidget(m_listWidget = new QTreeView(this));

	QStandardItemModel*    model = new QStandardItemModel();
	QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel();
	proxyModel->setSourceModel(model);
	proxyModel->setDynamicSortFilter(true);
	proxyModel->setFilterKeyColumn(CategoryColumn);
	m_listWidget->setRootIsDecorated(false);
	m_listWidget->setSortingEnabled(true);
	m_listWidget->setModel(proxyModel);
	m_listWidget->setEditTriggers(QTreeView::NoEditTriggers);
	m_listWidget->setIndentation(0);
	m_listWidget->setAlternatingRowColors(true);
	model->invisibleRootItem()->insertColumns(0, ColumnCount);
	model->setHeaderData(CategoryColumn,    Qt::Horizontal, tr("Category"));
	model->setHeaderData(FileNameColumn,    Qt::Horizontal, tr("File"));
	model->setHeaderData(LineNoColumn,      Qt::Horizontal, tr("Line"));
	model->setHeaderData(DescriptionColumn, Qt::Horizontal, tr("Description"));
	m_listWidget->setColumnWidth(CategoryColumn, 16 + 10/*‰½‚Ì’l‚¾‚ë‚¤H*/);
	m_listWidget->setColumnWidth(FileNameColumn, m_listWidget->fontMetrics().width(QLatin1Char('9')) * 20);
	m_listWidget->setColumnWidth(LineNoColumn,   m_listWidget->fontMetrics().width(QLatin1Char('9')) * 5);
	connect(m_listWidget, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedList(const QModelIndex &)));

	m_toolBar->setStyleSheet("QToolBar{border:none}");
	m_toolBar->setIconSize(QSize(16, 16));
	m_toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	m_visibledErrorAction      = m_toolBar->addAction(QIcon(":/images/tango/small/dialog-error.png"), "");
	                             m_toolBar->addSeparator();
	m_visibledWarningAction    = m_toolBar->addAction(QIcon(":/images/icons/small/software-update-urgent-yellow.png"), "");
	                             m_toolBar->addSeparator();
	m_visibledInfomationAction = m_toolBar->addAction(QIcon(":/images/tango/small/dialog-information.png"), "");
	m_visibledErrorAction->setCheckable(true);
	m_visibledErrorAction->setChecked(true);
	m_visibledWarningAction->setCheckable(true);
	m_visibledWarningAction->setChecked(true);
	m_visibledInfomationAction->setCheckable(true);
	m_visibledInfomationAction->setChecked(true);
	connect(m_visibledErrorAction,      SIGNAL(triggered()), this, SLOT(onUpdateCategoryVisibled()));
	connect(m_visibledWarningAction,    SIGNAL(triggered()), this, SLOT(onUpdateCategoryVisibled()));
	connect(m_visibledInfomationAction, SIGNAL(triggered()), this, SLOT(onUpdateCategoryVisibled()));
	updateMessagesCount();
}

void CMessageDock::updateMessagesCount()
{
	m_visibledErrorAction     ->setText(QString(tr("Error: %1 messages")).arg(m_countOfErrorMessages));
	m_visibledWarningAction   ->setText(QString(tr("Warning: %1 messages")).arg(m_countOfWarningMessages));
	m_visibledInfomationAction->setText(QString(tr("Information: %1 messages")).arg(m_countOfInfomationMessages));
}

void CMessageDock::clear()
{
	QStandardItemModel* model
		= static_cast<QStandardItemModel*>(
			static_cast<QSortFilterProxyModel*>(m_listWidget->model())->sourceModel()) ;
	if( model->rowCount() ) {
		model->removeRows(0, model->rowCount());
	}

	m_messages.clear();

	m_countOfErrorMessages = 0;
	m_countOfWarningMessages = 0;
	m_countOfInfomationMessages = 0;
	updateMessagesCount();
}

void CMessageDock::addMessage(const QUuid & uuid, int lineNo, CategoryType category, const QString & description)
{
	QString iconPath, categoryName;
	switch(category)
	{
	case InfomationCategory:
		iconPath     = ":/images/tango/small/dialog-information.png";
		categoryName = tr("Information");
		m_countOfInfomationMessages++;
		break;
	case WarningCategory:
		iconPath     = ":/images/icons/small/software-update-urgent-yellow.png";
		categoryName = tr("Warning");
		m_countOfWarningMessages++;
		break;
	case ErrorCategory:
		iconPath     = ":/images/tango/small/dialog-error.png";
		categoryName = tr("Error");
		m_countOfErrorMessages++;
		break;
	default:
		break;
	}

	QStandardItemModel* model
		= static_cast<QStandardItemModel*>(
			static_cast<QSortFilterProxyModel*>(m_listWidget->model())->sourceModel()) ;
	int row = model->rowCount();
	model->insertRow(row);
	if( !iconPath.isEmpty() ) {
		model->setData(model->index(row, CategoryColumn   ), QIcon(iconPath), Qt::DecorationRole);
		model->setData(model->index(row, CategoryColumn   ), categoryName);
	}
	model->setData(model->index(row, RefIndexColumn   ), m_messages.size(), Qt::UserRole + 1);
	model->setData(model->index(row, DescriptionColumn), description);
	model->setData(model->index(row, FileNameColumn   ), theFile.fileName(uuid));
	model->setData(model->index(row, LineNoColumn     ), 0 < lineNo ? QString("%1").arg(lineNo) : "");
	model->setData(model->index(row, LineNoColumn     ), Qt::AlignRight, Qt::TextAlignmentRole);

	MessageInfoType info;
	info.description= description;
	info.category	= category;
	info.uuid		= uuid;
	info.lineNo		= lineNo;
	m_messages.push_back(info);

	updateMessagesCount();
}

void CMessageDock::doubleClickedList(const QModelIndex & index)
{
	QSortFilterProxyModel* model
		= static_cast<QSortFilterProxyModel*>(m_listWidget->model()) ;
	QModelIndex sourceIndex = model->mapToSource(index);
	QStandardItem* item = static_cast<QStandardItem*>(sourceIndex.internalPointer());
	int indexOfInfo = item->child(sourceIndex.row(), RefIndexColumn)->data(Qt::UserRole + 1).toInt();
	MessageInfoType & info = m_messages[indexOfInfo];

	if( 0 < info.lineNo ) {
		emit gotoLine(info.uuid, info.lineNo);
	}
}

void CMessageDock::onUpdateCategoryVisibled()
{
	QSortFilterProxyModel* model
		= static_cast<QSortFilterProxyModel*>(m_listWidget->model()) ;

	QString re;

	if( m_visibledErrorAction->isChecked()      ) { re += "|" + tr("Error"); }
	if( m_visibledWarningAction->isChecked()    ) { re += "|" + tr("Warning"); }
	if( m_visibledInfomationAction->isChecked() ) { re += "|" + tr("Information"); }

	re = "^(" + re.mid(1) + ")$";

	model->setFilterRegExp(QRegExp(re));
}

