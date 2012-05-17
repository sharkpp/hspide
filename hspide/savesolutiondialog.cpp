#include <QtGui>
#include "savesolutiondialog.h"
#include "workspaceitem.h"
#include "documentpane.h"

CSaveSolutionDialog::CSaveSolutionDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	//QItemEditorFactory* factory;
	//QItemDelegate* itemDelegate;
	//ui.treeView->setItemDelegate(itemDelegate = new QItemDelegate);
	//	itemDelegate->setItemEditorFactory(factory = new QItemEditorFactory);
	//		factory->registerEditor(QVariant::Bool, new QStandardItemEditorCreator<QCheckBox>());
}

void CSaveSolutionDialog::clicked(QAbstractButton* button)
{
	int buttonType = (int)ui.buttonBox->standardButton(button);

	m_checkedItems.clear();

	if( QDialogButtonBox::Yes == buttonType )
	{
		// 「はい」の時のみ選択されている項目を列挙
		foreach(QStandardItem* item, m_items)
		{
			QStandardItemModel* model
				= qobject_cast<QStandardItemModel*>(ui.treeView->model());
			QModelIndex itemIndex = item->index();
			if( Qt::Checked == model->data(model->index(itemIndex.row(), SaveCheckColumn, itemIndex.parent()),
											Qt::CheckStateRole).toInt() )
			{
				m_checkedItems.push_back( (CWorkSpaceItem*)item->data().value<void*>() );
			}
		}
	}

	done(buttonType);
}

void CSaveSolutionDialog::canceled()
{
	done((int)QDialogButtonBox::Cancel);
}

bool CSaveSolutionDialog::setSolution(CWorkSpaceItem* item)
{
	QVector<QPair<CWorkSpaceItem*, int> > stack;
	stack.push_back(qMakePair(item, 0));

	QStandardItemModel* model;
	ui.treeView->setModel(model = new QStandardItemModel());
	QStandardItem* rootItem = model->invisibleRootItem();

	rootItem->setColumnCount(ColumnCount);

	ui.treeView->header()->setResizeMode(FileNameColumn,  QHeaderView::Stretch);
	ui.treeView->header()->setResizeMode(SaveCheckColumn, QHeaderView::ResizeToContents);

	QVector<QStandardItem*> stackItem;
	stackItem.push_back(rootItem);

	stackItem.push_back(new QStandardItem(item->text()));
	rootItem->appendRow(stackItem.back());

	m_items.push_back(stackItem.back());

	stackItem.back()->setData( qVariantFromValue((void*)item) );

	ui.treeView->setRowHidden(0, rootItem->index(), true);

	model->setData(model->index(stackItem.back()->index().row(), SaveCheckColumn, stackItem.back()->index().parent()),
					Qt::Unchecked, Qt::CheckStateRole);

	// ソリューション内で未保存のデータを列挙
	for(; !stack.isEmpty() ;)
	{
		CWorkSpaceItem* item_ = stack.back().first;
		int&            index = stack.back().second;

		if( item_->count() <= index ) {
			stackItem.pop_back();
			stack.pop_back();
			if( !stack.isEmpty() ) {
				stack.back().second++;
			}
		} else {
			CWorkSpaceItem*child    = item_->at(index);
			CDocumentPane* document = child->assignDocument();
			QStandardItem* newItem  = new QStandardItem(child->text());
			QStandardItem* curItem  = stackItem.back();
			bool defaultSaved       = document && document->isModified();

			// ワークスペースアイテムをツリーのアイテムからたどれるよう情報を保存
			newItem->setData( qVariantFromValue((void*)child) );

			curItem->appendRow(newItem);
			curItem->setColumnCount(ColumnCount);

			m_items.push_front(newItem);
			stackItem.push_back(newItem);
			stack.push_back(qMakePair(child, 0));

			// チェック状態を設定
			model->setData(model->index(newItem->index().row(), SaveCheckColumn, newItem->index().parent()),
				defaultSaved ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);

			QStandardItem* subItem = static_cast<QStandardItem*>(newItem->index().internalPointer())
										->child(newItem->index().row(), SaveCheckColumn);
			subItem->setFlags(subItem->flags()|Qt::ItemIsUserCheckable);

			if( !defaultSaved )
			{
				ui.treeView->setRowHidden(newItem->row(), newItem->parent()->index(), true);
			}
			else
			{
				// 親をたどって表示されていないアイテムを無効状態にする
				for(;curItem && curItem->index().isValid(); curItem = curItem->parent())
				{
					if( ui.treeView->isRowHidden(curItem->row(), curItem->index().parent()) )
					{
						// 非表示のものを表示する
						ui.treeView->setRowHidden(curItem->row(), curItem->index().parent(), false);
						// 操作不可にする
						curItem->setEnabled(false);
						static_cast<QStandardItem*>(curItem->index().internalPointer())
							->child(curItem->index().row(), SaveCheckColumn)->setEnabled(false);
					}
				}
			}
		}
	}

	ui.treeView->expandAll();

	return true;
}

QList<CWorkSpaceItem*> CSaveSolutionDialog::list()
{
	return m_checkedItems;
}
