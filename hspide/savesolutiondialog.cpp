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
	WorkSpaceItemRole = Qt::UserRole + 1, // CWorkspaceItem�ւ̃|�C���^��ێ�
	DisableIndexRole, // �w��C���f�b�N�X�̌��������w��
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
		// �ۑ���ʂ̑I���J�����̂݃G�f�B�^��L���ɂ���
		if( SaveKindColumn == index.column() &&
			!index.data(Qt::DisplayRole).isNull() )
		{
			QWidget* editor = QItemDelegate::createEditor(parent, option, index);
			if( !index.data(DisableIndexRole).isNull() ) {
				// �w��̑I�����ڂ𖳌����I��s�ɂ���
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
			// �f�[�^�Ƃ��ē����Ă���͍̂��ڂ̃C���f�b�N�X�Ȃ̂ŃC���f�b�N�X����\����������擾
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

	// �ۑ���ʂ̑I�����ڂ𐶐�
	QStringList itemList;
	itemList<< tr("Without save")
			<< tr("Overwrite save")
			<< tr("Save as");
	// �ۑ���ʂ��R���{�{�b�N�X�őI�ׂ�悤�ɃG�f�B�^�����蓖��
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
		// �u�͂��v�̎��̂ݑI������Ă��鍀�ڂ��
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
	if( item->isUntitled() ) { // ����̏ꍇ�㏑���ۑ��𖳌��ɂ���
		newItem->setData(SaveKindColumn, DisableIndexRole, qVariantFromValue<int>(OverwriteSave));
		newItem->setData(SaveKindColumn, Qt::DisplayRole,  qVariantFromValue<int>(SaveAs));
	} else {
		newItem->setData(SaveKindColumn, Qt::DisplayRole,  qVariantFromValue<int>(OverwriteSave));
	}

	m_items.push_back(newItem);
	stackItem.push_back(newItem);

	// �\�����[�V�������Ŗ��ۑ��̃f�[�^���
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

			// ���[�N�X�y�[�X�A�C�e�����c���[�̃A�C�e�����炽�ǂ��悤����ۑ�
			curItem->addChild(newItem = new QTreeWidgetItem());
			newItem->setText(FileNameColumn, child->text());
			newItem->setData(FileNameColumn, WorkSpaceItemRole, qVariantFromValue((void*)child) );
			newItem->setFlags(newItem->flags()|Qt::ItemIsEditable);

			// �t���b�g�ŊǗ����邽�߂Ƀ��X�g�ɒǉ�
			m_items.push_front(newItem);
			// �X�^�b�N�Ƀv�b�V�����ăc���[���Ǘ�
			stackItem.push_back(newItem);
			stack.push_back(qMakePair(child, 0));

			// �`�F�b�N��Ԃ�ݒ�

			if( !defaultSaved )
			{
				newItem->setHidden(true);
			}
			else
			{
				noSaveItem = false;

				if( document->isUntitled() ) { // ����̏ꍇ�㏑���ۑ��𖳌��ɂ���
					newItem->setData(SaveKindColumn, DisableIndexRole, qVariantFromValue<int>(OverwriteSave));
					newItem->setData(SaveKindColumn, Qt::DisplayRole,  qVariantFromValue<int>(SaveAs));
				} else {
					newItem->setData(SaveKindColumn, Qt::DisplayRole,  qVariantFromValue<int>(OverwriteSave));
				}

				// �e�����ǂ��ĕ\������Ă��Ȃ��A�C�e���𖳌���Ԃɂ���
				for(;curItem ; curItem = curItem->parent())
				{
					if( curItem->isHidden() )
					{
						// ��\���̂��̂�\��������s�ɂ���
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
