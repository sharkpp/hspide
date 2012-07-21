#include <QtGui>
#include "savesolutiondialog.h"
#include "documentpane.h"

typedef enum {
	FileNameColumn,
	SaveKindColumn,
	ColumnCount,
};

typedef enum {
	WithoutSave,
	OverwriteSave,
	SaveAs,
};

typedef enum {
	WorkSpaceItemRole = Qt::UserRole + 1, // CWorkspaceItemへのポインタを保持
	DisableIndexRole, // 指定インデックスの向こうを指示
};

class CSaveKindItemDelegate
	: public QItemDelegate
{
    QStringList m_itemList;
public:
	CSaveKindItemDelegate(QObject* parent, const QStringList& itemList)
		: QItemDelegate(parent)
		, m_itemList(itemList)
	{}
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
		// 保存種別の選択カラムのみエディタを有効にする
		if( SaveKindColumn == index.column() &&
			!index.data(Qt::DisplayRole).isNull() )
		{
			QWidget* editor = QItemDelegate::createEditor(parent, option, index);
			if( !index.data(DisableIndexRole).isNull() ) {
				// 指定の選択項目を無効＆選択不可にする
				int disableIndex = index.data(DisableIndexRole).toInt();
				if( QComboBox* combobox = qobject_cast<QComboBox*>(editor) ) {
					QStandardItemModel* model = qobject_cast<QStandardItemModel*>(combobox->model());
					if( QStandardItem* item = model->invisibleRootItem()->child(disableIndex, 0) ) {
						item->setSelectable(false);
						item->setEnabled(false);
					}
				}
			}
			return editor;
		}
		return 0;
	}
 	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
		QSize size = QItemDelegate::sizeHint(option, index);
		size.setHeight(size.height() + 4);
		return size;
	}
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
		if( SaveKindColumn != index.column() ) {
			QItemDelegate::paint(painter, option, index);
		} else {
			// データとして入っているのは項目のインデックスなのでインデックスから表示文字列を取得
			QVariant itemData = index.data(Qt::DisplayRole);
			QString text = !itemData.isNull() ? m_itemList.at(itemData.toInt()) : QString();
			QRect displayRect, decorationRect, checkRect;
			QItemDelegate::doLayout(option, &checkRect, &decorationRect, &displayRect, false);
			QItemDelegate::drawBackground(painter, option, index);
			QItemDelegate::drawDisplay(painter, option, displayRect, text);
			QItemDelegate::drawFocus(painter, option, displayRect);
		}
	}
};

class CSaveKindItemEditorCreator
	: public QItemEditorCreatorBase
{
    QStringList m_itemList;
public:
	CSaveKindItemEditorCreator(const QStringList& itemList)
		: m_itemList(itemList)
    {}
    QWidget *createWidget(QWidget *parent) const {
		QComboBox* editor = new QComboBox(parent);
		foreach(const QString& text, m_itemList) {
			editor->addItem(text);
		}
		return editor;
	}
    QByteArray valuePropertyName() const {
		return QByteArray("currentIndex");
	}
};

CSaveSolutionDialog::CSaveSolutionDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	// 保存種別の選択項目を生成
	QStringList itemList;
	itemList<< tr("Without save")
			<< tr("Overwrite save")
			<< tr("Save as");
	// 保存種別をコンボボックスで選べるようにエディタを割り当て
	QItemEditorFactory* factory;
	QItemDelegate* itemDelegate;
	ui.saveTargetList->setItemDelegate(itemDelegate = new CSaveKindItemDelegate(this, itemList));
		itemDelegate->setItemEditorFactory(factory = new QItemEditorFactory);
			factory->registerEditor(QVariant::Int, new CSaveKindItemEditorCreator(itemList));
}

void CSaveSolutionDialog::clicked(QAbstractButton* button)
{
	int buttonType = (int)ui.buttonBox->standardButton(button);

	m_savingItems.clear();

	if( QDialogButtonBox::Yes == buttonType )
	{
		// 「はい」の時のみ選択されている項目を列挙
		foreach(QTreeWidgetItem* item, m_items)
		{
			QVariant data = item->data(SaveKindColumn, Qt::DisplayRole);
			if( !data.isNull() &&
				WithoutSave != data.toInt() )
			{
				CWorkSpaceItem* wsItem
					= static_cast<CWorkSpaceItem*>(item->data(FileNameColumn, WorkSpaceItemRole).value<void*>());
				CWorkSpaceItem::SaveType saveType;
				switch( data.toInt() ) {
					default:
					case OverwriteSave: saveType = CWorkSpaceItem::OverwriteSave; break;
					case SaveAs:        saveType = CWorkSpaceItem::SaveAs; break;
				}
				m_savingItems.push_back( qMakePair(wsItem, saveType) );
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
	bool noSaveItem = true;

	QVector<QTreeWidgetItem*> stackItem;
	QVector<QPair<CWorkSpaceItem*, int> > stack;
	stack.push_back(qMakePair(item, 0));

	ui.saveTargetList->setColumnCount(ColumnCount);
	ui.saveTargetList->header()->setResizeMode(FileNameColumn,  QHeaderView::Stretch);
	ui.saveTargetList->header()->setResizeMode(SaveKindColumn, QHeaderView::ResizeToContents);

	QTreeWidgetItem *rootItem = ui.saveTargetList->invisibleRootItem();
	stackItem.push_back(rootItem);

	QTreeWidgetItem *newItem;
	rootItem->addChild(newItem = new QTreeWidgetItem());
	newItem->setText(FileNameColumn, item->text());
	newItem->setData(FileNameColumn, WorkSpaceItemRole, qVariantFromValue((void*)item) );
//	newItem->setDisabled(false);
	newItem->setHidden(true);
	newItem->setFlags(newItem->flags()|Qt::ItemIsEditable);
	if( item->isUntitled() ) { // 無題の場合上書き保存を無効にする
		newItem->setData(SaveKindColumn, DisableIndexRole, qVariantFromValue<int>(OverwriteSave));
		newItem->setData(SaveKindColumn, Qt::DisplayRole,  qVariantFromValue<int>(SaveAs));
	} else {
		newItem->setData(SaveKindColumn, Qt::DisplayRole,  qVariantFromValue<int>(OverwriteSave));
	}

	m_items.push_back(newItem);
	stackItem.push_back(newItem);

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
			QTreeWidgetItem* curItem= stackItem.back();
			bool defaultSaved       = document && document->isModified();

			// ワークスペースアイテムをツリーのアイテムからたどれるよう情報を保存
			curItem->addChild(newItem = new QTreeWidgetItem());
			newItem->setText(FileNameColumn, child->text());
			newItem->setData(FileNameColumn, WorkSpaceItemRole, qVariantFromValue((void*)child) );
			newItem->setFlags(newItem->flags()|Qt::ItemIsEditable);

			// フラットで管理するためにリストに追加
			m_items.push_front(newItem);
			// スタックにプッシュしてツリーを管理
			stackItem.push_back(newItem);
			stack.push_back(qMakePair(child, 0));

			// チェック状態を設定

			if( !defaultSaved )
			{
				newItem->setHidden(true);
			}
			else
			{
				noSaveItem = false;

				if( document->isUntitled() ) { // 無題の場合上書き保存を無効にする
					newItem->setData(SaveKindColumn, DisableIndexRole, qVariantFromValue<int>(OverwriteSave));
					newItem->setData(SaveKindColumn, Qt::DisplayRole,  qVariantFromValue<int>(SaveAs));
				} else {
					newItem->setData(SaveKindColumn, Qt::DisplayRole,  qVariantFromValue<int>(OverwriteSave));
				}

				// 親をたどって表示されていないアイテムを無効状態にする
				for(;curItem ; curItem = curItem->parent())
				{
					if( curItem->isHidden() )
					{
						// 非表示のものを表示＆操作不可にする
						curItem->setHidden(false);
	//					curItem->setDisabled(true);
					}
				}
			}
		}
	}

	ui.saveTargetList->expandAll();

	return !noSaveItem;
}

QList<CSaveSolutionDialog::SavingItemInfo> CSaveSolutionDialog::list()
{
	return m_savingItems;
}
