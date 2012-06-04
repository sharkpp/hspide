#include <QResizeEvent>
#include <QtGui>
#include "breakpointdock.h"
#include "global.h"

typedef enum {
	UuidRole = Qt::UserRole + 1,	// ファイルに対する一意なID
	LineNumberRole,					// 行番号
};

class CExpandedItemHeightDelegate
	: public QItemDelegate
{
public:
	CExpandedItemHeightDelegate(QObject* parent = 0) : QItemDelegate(parent) {}
	QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const {
		QSize size = QItemDelegate::sizeHint(option, index);
		size.setHeight(size.height() + 4);
		return size;
	}
};

CBreakPointDock::CBreakPointDock(QWidget *parent)
	: QWidget(parent)
{
	QItemEditorFactory* factory;
	QItemDelegate* itemDelegate;

	QStringList labels;
	labels	<< tr("Name")
			;

	m_listWidget = new QTreeWidget(this);
	m_listWidget->setRootIsDecorated(false);
	m_listWidget->setIndentation(10);
	m_listWidget->setEditTriggers(QTreeView::NoEditTriggers);
	m_listWidget->setColumnCount(ColumnCount);
	m_listWidget->setHeaderLabels(labels);
	m_listWidget->setItemDelegate(itemDelegate = new CExpandedItemHeightDelegate);
//	setEnable(false);

	connect(&theBreakPoint, SIGNAL(breakPointChanged(const QUuid&, const QList<QPair<int, bool> >&)),
	        this,  SLOT(onBreakPointChanged(const QUuid&, const QList<QPair<int, bool> >&)));

	connect(m_listWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
			this, SLOT(onBreakPointStateChanged(QTreeWidgetItem*,int)));
	connect(m_listWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
			this, SLOT(onBreakPointDoubleClicked(QTreeWidgetItem*,int)));
}

void CBreakPointDock::resizeEvent(QResizeEvent * event)
{
	m_listWidget->resize(event->size());
}

void CBreakPointDock::setEnable(bool enable)
{
	m_listWidget->setEnabled(enable);
}

void CBreakPointDock::onBreakPointChanged(const QUuid& uuid, const QList<QPair<int, bool> >& modifiedLineNumberes)
{
	QTreeWidgetItem* rootItem = m_listWidget->invisibleRootItem();
	QTreeWidgetItem* fileItem = NULL;
	QTreeWidgetItem* lineItem = NULL;

	// すでに登録済み？
	for(int i = 0, num = rootItem->childCount();
		i < num && !fileItem; ++i)
	{
		QTreeWidgetItem* item = rootItem->child(i);
		if( uuid == QUuid(item->data(FileColumn, UuidRole).toString()) )
		{
			fileItem = item;
		}
	}
	if( !fileItem )
	{
		rootItem->addChild(fileItem = new QTreeWidgetItem(QStringList(theFile.fileName(uuid))));
		fileItem->setData(FileColumn, UuidRole,       uuid.toString());
		fileItem->setData(FileColumn, LineNumberRole, -1);
		fileItem->setCheckState(FileColumn, Qt::Checked);
	}

	// 行を探す
	for(QList<QPair<int, bool> >::const_iterator
			ite = modifiedLineNumberes.begin(),
			last= modifiedLineNumberes.end();
		ite != last; ++ite)
	{
		int insertLine = 0;
		for(int i = 0, num = fileItem->childCount();
			i < num && !lineItem; ++i)
		{
			QTreeWidgetItem* item = fileItem->child(i);
			int lineNum = item->data(FileColumn, LineNumberRole).toInt();
			if( ite->first == lineNum )
			{
				lineItem = item;
			}
			if( lineNum <= ite->first )
			{
				insertLine = i + 1;
			}
		}

		if( ite->second )
		{
			if( !lineItem )
			{
				fileItem->insertChild(insertLine, lineItem = new QTreeWidgetItem(QStringList(QString("%1").arg(ite->first))));
				lineItem->setData(FileColumn, UuidRole,       uuid.toString());
				lineItem->setData(FileColumn, LineNumberRole, ite->first);
				lineItem->setCheckState(FileColumn, Qt::Checked);
			}
		}
		else
		{
			if( lineItem )
			{
				fileItem->removeChild(lineItem);
			}
		}
	}

	// 行がなければファイルも削除
	if( !fileItem->childCount() )
	{
		rootItem->removeChild(fileItem);
	}

	// ツリーを全部展開
	m_listWidget->expandAll();
}

void CBreakPointDock::onBreakPointStateChanged(QTreeWidgetItem* item, int column)
{
	QUuid uuid(item->data(FileColumn, UuidRole).toString());
	int lineNo = item->data(FileColumn, LineNumberRole).toInt();
}

void CBreakPointDock::onBreakPointDoubleClicked(QTreeWidgetItem* item, int column)
{
	QUuid uuid(item->data(FileColumn, UuidRole).toString());
	int lineNo = item->data(FileColumn, LineNumberRole).toInt();

	emit gotoLine(uuid, lineNo);
}
