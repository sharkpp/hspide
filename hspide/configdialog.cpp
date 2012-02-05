#include "configdialog.h"
#include <QtGui>

CConfigDialog::CConfigDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	// Ý’è‰æ–Ê‚Ìƒy[ƒWˆê——‚ð“o˜^
	QStandardItemModel* model = new QStandardItemModel();
	ui.category->setRootIsDecorated(false);
	ui.category->setHeaderHidden(true);
	ui.category->setModel(model);
	ui.category->setEditTriggers(QTreeView::NoEditTriggers);
	QStandardItem* rootItem = model->invisibleRootItem();
	QStandardItem* item, *item2;
	rootItem->appendRow(item = new QStandardItem(tr("Directory")));
		item->setData(ui.stackedWidget->indexOf(ui.directoryPage));
	rootItem->appendRow(item = new QStandardItem(tr("Editor")));
		item->appendRow(item2 = new QStandardItem(tr("General")));
			item2->setData(ui.stackedWidget->indexOf(ui.editorGeneralPage));
		item->appendRow(item2 = new QStandardItem(tr("Color")));
			item2->setData(ui.stackedWidget->indexOf(ui.editorColorPage));
	rootItem->appendRow(item = new QStandardItem(tr("Tools")));
		item->setData(ui.stackedWidget->indexOf(ui.toolsPage));
	rootItem->appendRow(item = new QStandardItem(tr("Shortcut key")));
		item->setData(ui.stackedWidget->indexOf(ui.shortcutKeyPage));

	ui.category->expandAll();

	ui.stackedWidget->setCurrentIndex(0);
}

void CConfigDialog::onPageChanged(const QModelIndex & index)
{
	QStandardItem* item = static_cast<QStandardItem*>(index.internalPointer());
	               item = item->child(index.row(), 0);
	if( item->data().isValid() )
	{
		ui.stackedWidget->setCurrentIndex(item->data().toInt());
	}
	else
	{
		QModelIndex newIndex = item->child(0)->index();
		ui.category->setCurrentIndex(newIndex);
		onPageChanged(newIndex);
	}
}

