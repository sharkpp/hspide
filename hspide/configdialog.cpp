#include "configdialog.h"
#include "hotkeywidget.h"
#include "savepresetdialog.h"
#include "global.h"
#include <QtGui>

class CExpandedItemHeightDelegate
	: public QItemDelegate
{
public:
	CExpandedItemHeightDelegate(QObject* parent = 0) : QItemDelegate(parent) {}
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const {
		QSize size = QItemDelegate::sizeHint(option, index);
		size.setHeight(size.height() + 4);
		return size;
	}
};

class CKeyAssignListItemDelegate
	: public CExpandedItemHeightDelegate
{
public:
	CKeyAssignListItemDelegate(QObject* parent = 0) : CExpandedItemHeightDelegate(parent) {}
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
		if( 1 == index.column() ) {
			return QItemDelegate::createEditor(parent, option, index);
		}
		return 0;
	}
};

CConfigDialog::CConfigDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::ConfigDialog)
    , m_lastIndexOfColorMetrics(Configuration::ColorMetricsNum)
{
	ui->setupUi(this);

	QStandardItemModel* model;
	QStandardItem* rootItem;
	QStandardItem* item, *item2;
	QItemEditorFactory* factory;
	QItemDelegate* itemDelegate;

	// 設定画面のページ一覧を登録
	ui->category->setRootIsDecorated(false);
	ui->category->setHeaderHidden(true);
	ui->category->setModel(model = new QStandardItemModel());
	rootItem = model->invisibleRootItem();
	rootItem->appendRow(item = new QStandardItem(tr("Directory")));
		item->setData(ui->stackedWidget->indexOf(ui->directoryPage));
	rootItem->appendRow(item = new QStandardItem(tr("Build configurations")));
		item->setData(ui->stackedWidget->indexOf(ui->buildConfigurationsPage));
	rootItem->appendRow(item = new QStandardItem(tr("Editor")));
		item->appendRow(item2 = new QStandardItem(tr("General")));
			item2->setData(ui->stackedWidget->indexOf(ui->editorGeneralPage));
		item->appendRow(item2 = new QStandardItem(tr("Color")));
			item2->setData(ui->stackedWidget->indexOf(ui->editorColorPage));
	rootItem->appendRow(item = new QStandardItem(tr("Tools")));
		item->setData(ui->stackedWidget->indexOf(ui->toolsPage));
	rootItem->appendRow(item = new QStandardItem(tr("Key assign")));
		item->setData(ui->stackedWidget->indexOf(ui->keyAssignPage));
	rootItem->appendRow(item = new QStandardItem(tr("Toolbar")));
		item->setData(ui->stackedWidget->indexOf(ui->toolbarPage));
	ui->category->expandAll();

	// ビルド構成一覧を初期化

	// 色設定の一覧を初期化
	QVector<QPair<Qt::GlobalColor, QString> > colorValues;
	colorValues	<< qMakePair(Qt::white,       tr("White"))
				<< qMakePair(Qt::black,       tr("Black"))
				<< qMakePair(Qt::red,         tr("Red"))
				<< qMakePair(Qt::darkRed,     tr("Maroon"))
				<< qMakePair(Qt::green,       tr("Lime"))
				<< qMakePair(Qt::darkGreen,   tr("Green"))
				<< qMakePair(Qt::blue,        tr("Blue"))
				<< qMakePair(Qt::darkBlue,    tr("Navy"))
				<< qMakePair(Qt::cyan,        tr("Aqua"))
				<< qMakePair(Qt::darkCyan,    tr("Teal"))
				<< qMakePair(Qt::magenta,     tr("Fuchsia"))
				<< qMakePair(Qt::darkMagenta, tr("Purple"))
				<< qMakePair(Qt::yellow,      tr("Yellow"))
				<< qMakePair(Qt::darkYellow,  tr("Olive"))
				<< qMakePair(Qt::gray,        tr("Light gray"))
				<< qMakePair(Qt::darkGray,    tr("Gray"))
				<< qMakePair(Qt::lightGray,   tr("Silver"))
				;
	QMenu* menu1 = new QMenu(this);
	QMenu* menu2 = new QMenu(this);
	QAction* action1, *action2;
		for(int i = 0; i < colorValues.size(); i++)
		{
			QPixmap bmp(16, 16);
			bmp.fill(colorValues[i].first);
			action1 = menu1->addAction(QIcon(bmp), colorValues[i].second);
			action2 = menu2->addAction(QIcon(bmp), colorValues[i].second);
			action1->setData(int(colorValues[i].first));
			action2->setData(int(colorValues[i].first));
			connect(action1, SIGNAL(triggered()), this, SLOT(onChangedMetricsBgcolor()));
			connect(action2, SIGNAL(triggered()), this, SLOT(onChangedMetricsFgcolor()));
		}
		action1 = menu1->addAction(tr("Custom ..."));
		action2 = menu2->addAction(tr("Custom ..."));
		action1->setData(-1);
		action2->setData(-1);
		connect(action1, SIGNAL(triggered()), this, SLOT(onChangedMetricsBgcolor()));
		connect(action2, SIGNAL(triggered()), this, SLOT(onChangedMetricsFgcolor()));
	ui->editorColorItemBgcolor->setMenu(menu1);
	ui->editorColorItemFgcolor->setMenu(menu2);

	ui->editorColorList->setColumnCount(ColorListColumnNum);
	ui->editorColorList->header()->setResizeMode(ColorListCategoryColumn,        QHeaderView::Stretch);
	ui->editorColorList->header()->setResizeMode(ColorListEnableColumn,          QHeaderView::ResizeToContents);
	ui->editorColorList->header()->setResizeMode(ColorListFontBoldColumn,        QHeaderView::ResizeToContents);
    ui->editorColorList->header()->setResizeMode(ColorListForegroundColorColumn, QHeaderView::ResizeToContents);
	ui->editorColorList->header()->setResizeMode(ColorListBackgroundColorColumn, QHeaderView::ResizeToContents);
	ui->editorColorList->setItemDelegate(itemDelegate = new CExpandedItemHeightDelegate);

	// ショートカットの一覧を初期化
	ui->keyAssignList->setColumnCount(KeyAssignListColumnNum);
	ui->keyAssignList->header()->setResizeMode(KeyAssignListNameColumn,        QHeaderView::Stretch);
	ui->keyAssignList->header()->setResizeMode(KeyAssignListKeyAssignKeyColumn, QHeaderView::ResizeToContents);
	ui->keyAssignList->setItemDelegate(itemDelegate = new CKeyAssignListItemDelegate);
		itemDelegate->setItemEditorFactory(factory = new QItemEditorFactory);
			factory->registerEditor(QVariant::KeySequence, new QStandardItemEditorCreator<CHotkeyWidget>());

	ui->category->selectionModel()->clear();
	ui->category->selectionModel()->select(rootItem->child(0)->index(), QItemSelectionModel::Select|QItemSelectionModel::Current);
	ui->stackedWidget->setCurrentIndex(0);

	// ツールバー
	ui->toolbarPartsList->setItemDelegate(itemDelegate = new CExpandedItemHeightDelegate);
	ui->toolbarCurrentList->setItemDelegate(itemDelegate = new CExpandedItemHeightDelegate);

	setConfiguration(theConf);
}

void CConfigDialog::setConfiguration(const Configuration* conf)
{
	struct {
		QString   iconPath;
		QString   text;
	} actionInitTable[Configuration::ActionNum + 1] = {
		/* ActionNew                 */{ ":/images/tango/small/document-new.png",          tr("New")                   },
		/* ActionOpen                */{ ":/images/tango/small/document-open.png",         tr("Open")                  },
		/* ActionSave                */{ ":/images/tango/small/document-save.png",         tr("Save")                  },
		/* ActionSaveAs              */{ ":/images/tango/small/document-save-as.png",      tr("Save as")               },
		/* ActionSaveAll             */{ ":/images/icons/small/document-save-all.png",     tr("Save all")              },
		/* ActionQuit                */{ ":/images/tango/small/system-log-out.png",        tr("Quit")                  },
		/* ActionUndo                */{ ":/images/tango/small/edit-undo.png",             tr("Undo")                  },
		/* ActionRedo                */{ ":/images/tango/small/edit-redo.png",             tr("Redo")                  },
		/* ActionCut                 */{ ":/images/tango/small/edit-cut.png",              tr("Cut")                   },
		/* ActionCopy                */{ ":/images/tango/small/edit-copy.png",             tr("Copy")                  },
		/* ActionPaste               */{ ":/images/tango/small/edit-paste.png",            tr("Paste")                 },
		/* ActionClear               */{ ":/images/tango/small/edit-clear.png",            tr("Clear")                 },
		/* ActionSelectAll           */{ ":/images/tango/small/edit-select-all.png",       tr("Select all")            },
		/* ActionFind                */{ ":/images/tango/small/edit-find.png",             tr("Find")                  },
		/* ActionFindNext            */{ ":/images/tango/small/edit-select-all.png",       tr("Find next")             },
		/* ActionFindPrev            */{ ":/images/tango/small/edit-select-all.png",       tr("Find previous")         },
		/* ActionReplace             */{ ":/images/tango/small/edit-find-replace.png",     tr("Replace")               },
		/* ActionJump                */{ ":/images/tango/small/go-jump.png",               tr("Jump")                  },
		/* ActionBuildSolution       */{ "",                                               tr("Build solution")        },
		/* ActionBuildProject        */{ "",                                               tr("Build project")         },
		/* ActionCompileOnly         */{ "",                                               tr("Compile only")          },
		/* ActionBatchBuild          */{ "",                                               tr("Batch build")           },
		/* ActionDebugRunSolution    */{ ":/images/tango/small/media-playback-start.png",  tr("Debug run solution")    },
		/* ActionNoDebugRunSolution  */{ "",                                               tr("No debug run solution") },
		/* ActionDebugRunProject     */{ ":/images/icons/small/run-project.png",           tr("Debug run project")     },
		/* ActionNoDebugRunProject   */{ "",                                               tr("No debug run project")  },
		/* ActionDebugSuspend        */{ ":/images/tango/small/media-playback-pause.png",  tr("Debug suspend")         },
		/* ActionDebugResume         */{ ":/images/tango/small/media-playback-start.png",  tr("Debug resume")          },
		/* ActionDebugStop           */{ ":/images/tango/small/media-playback-stop.png",   tr("Debug stop")            },
		/* ActionDebugStepIn         */{ ":/images/icons/small/step-in.png",               tr("Step in")               },
		/* ActionDebugStepOver       */{ ":/images/icons/small/step-over.png",             tr("Step over")             },
		/* ActionDebugStepOut        */{ "",                                               tr("Step out")              },
		/* ActionBreakpointSet       */{ ":/images/icons/small/breakpoint.png",            tr("Set/reset breakpoint")  },
		/* ActionBuildTarget         */{ "",                                               tr("Build configurations")  },
		/* ActionConfig              */{ ":/images/tango/small/preferences-system.png",    tr("Configuration")         },
		/* ActionShowWorkSpace       */{ "",                                               tr("Show workspace")        },
		/* ActionShowSymbol          */{ "",                                               tr("Show symbols")          },
		/* ActionShowOutput          */{ "",                                               tr("Show output")           },
		/* ActionShowSearch          */{ "",                                               tr("Show search")           },
		/* ActionShowMessage         */{ "",                                               tr("Show messages")         },
		/* ActionShowBreakPoint      */{ "",                                               tr("Show breakpoints")      },
		/* ActionShowSysInfo         */{ "",                                               tr("Show system variables") },
		/* ActionShowVarInfo         */{ "",                                               tr("Show variables")        },
		/* ActionAbout               */{ ":/images/tango/small/dialog-information.png",    tr("About")                 },
	// 区切り ----------------
		/*                           */{ "",                                               tr("----")                  },
	};

	m_configuration = *conf;

	// 「ディレクトリ」ページ初期化

	ui->installDirectoryPath->setText(m_configuration.hspPath());
	ui->commonDirectoryPath->setText(m_configuration.hspCommonPath());

	// 「ビルド構成」ページ初期化
	const Configuration::BuildConfList& buildConf = m_configuration.buildConf();
	Configuration::BuildConfType tmp = { QString() };
	updateBuildConf(!buildConf.isEmpty() ? buildConf.at(0) : tmp);
	updateBuildConfPresetList();

	// 「エディタ - 一般」ページ初期化

	ui->editorTabWidth->setValue(m_configuration.editorTabWidth());
	ui->editorEnableLineNumber->setCheckState(m_configuration.editorLineNumberVisibled() ? Qt::Checked : Qt::Unchecked);
	ui->editorLineNumberFont->setCurrentFont(QFont(m_configuration.editorLineNumberFontName()));
	ui->editorLineNumberFontSize->setValue(m_configuration.editorLineNumberFontSize());
	ui->editorFont->setCurrentFont(QFont(m_configuration.editorFontName()));
	ui->editorFontSize->setValue(m_configuration.editorFontSize());

	// 「エディタ - 色」ページ初期化

    m_lastIndexOfColorMetrics = Configuration::ColorMetricsNum;
    QPixmap bmp(12, 12);
	QStringList categoryList;
	categoryList<< tr("Text")
				<< tr("Line number")
				<< tr("Function")
				<< tr("Sub routine")
				<< tr("Preprocessor")
				<< tr("Macro")
				<< tr("Label")
				<< tr("Comment")
				<< tr("Single quoted strings")
				<< tr("Double quoted strings")
				<< tr("Number")
				<< tr("Line feed char")
				<< tr("Tab char")
				<< tr("End of file")
				;
	ui->editorColorList->clear();
	ui->editorColorList->blockSignals(true);
	for(int i = 0; i < categoryList.size(); i++)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(ui->editorColorList);
		const Configuration::ColorMetricsInfoType& metricsInfo = m_configuration.colorMetrics(Configuration::ColorMetricsEnum(i));
		item->setText(ColorListCategoryColumn,                   categoryList.at(i));
		item->setCheckState(ColorListEnableColumn,               metricsInfo.enable      ? Qt::Checked : Qt::Unchecked);
		item->setCheckState(ColorListFontBoldColumn,             metricsInfo.useBoldFont ? Qt::Checked : Qt::Unchecked);
        item->setData(ColorListForegroundColorColumn, Qt::UserRole + 1, metricsInfo.foregroundColor);
        item->setData(ColorListBackgroundColorColumn, Qt::UserRole + 1, metricsInfo.backgroundColor);
        item->setIcon(ColorListForegroundColorColumn, (bmp.fill(metricsInfo.foregroundColor), QIcon(bmp)));
		item->setIcon(ColorListBackgroundColorColumn, (bmp.fill(metricsInfo.backgroundColor), QIcon(bmp)));
    }
	ui->editorColorList->blockSignals(false);
	ui->editorColorList->resizeColumnToContents(ColorListEnableColumn);
	ui->editorColorList->resizeColumnToContents(ColorListFontBoldColumn);
	ui->editorColorList->setCurrentItem(ui->editorColorList->topLevelItem(0));

	// 「キー割り当て」ページ初期化

	ui->keyAssignList->clear();
	for(size_t i = 0; i < _countof(actionInitTable) - 1; i++)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(ui->keyAssignList);
		item->setText(KeyAssignListNameColumn, actionInitTable[i].text);
		item->setFlags(item->flags()|Qt::ItemIsEditable);
	}

	updateKeyAssign(m_configuration.keyAssign());
	updateKeyAssignPresetList();

	// ツールバー初期化

	ui->toolbarPartsList->clear();
	for(size_t i = 0; i < _countof(actionInitTable); i++)
	{
		QListWidgetItem *item = new QListWidgetItem(ui->toolbarPartsList);
		item->setText(actionInitTable[i].text);
		if( !actionInitTable[i].iconPath.isEmpty() ) {
			item->setIcon(QIcon(actionInitTable[i].iconPath));
		} else {
			item->setIcon(QIcon(":/images/icons/small/blank.png"));
		}
		item->setData(Qt::UserRole + 1, (int)Configuration::ActionEnum(i));
	//	item->setHidden(Configuration::ActionNum <= i);
	}
	ui->toolbarPartsList->setCurrentRow(0);

	updateToolbar(m_configuration.toolbar());
	updateToolbarPresetList();

}

void CConfigDialog::updateBuildConf(const Configuration::BuildConfType& buildConf)
{
	ui->buildConfigurationsPageContents->setBuildConf(buildConf);
}

void CConfigDialog::applyBuildConf(Configuration::BuildConfType& buildConf)
{
	buildConf = ui->buildConfigurationsPageContents->buildConf();
}

void CConfigDialog::updateBuildConfPresetList()
{
	const Configuration::BuildConfList& buildConf = m_configuration.buildConf();
	ui->buildConfigurationsPresetList->blockSignals(true);
	for(int i = ui->buildConfigurationsPresetList->count();
		i < 1 + buildConf.size(); i++)
	{
		ui->buildConfigurationsPresetList->addItem("");
	}
	for(int i = 1 + buildConf.size();
		i < ui->buildConfigurationsPresetList->count(); i++)
	{
		ui->buildConfigurationsPresetList->removeItem(ui->buildConfigurationsPresetList->count() - 1);
	}
	ui->buildConfigurationsPresetList->blockSignals(false);

	// 選択が無ければ「現在の設定」を選択
	int curPreset = ui->buildConfigurationsPresetList->currentIndex();
	if( curPreset < 0 ) {
		ui->buildConfigurationsPresetList->setCurrentIndex(0);
		curPreset = 0;
	}

	// 変更をチェック
	Configuration::BuildConfType buildConfCurrent;
	applyBuildConf(buildConfCurrent);
	bool presetModified = curPreset < buildConf.size() && buildConfCurrent != buildConf[curPreset];

	ui->buildConfigurationsPresetRemove->setDisabled(buildConf.size() <= curPreset);

	QString presetName;
	for(int i = 0; i <= buildConf.size(); i++)
	{
		if( buildConf.size() <= i )
		{
			presetName = tr("(New setting)");
		}
		else
		{
			presetName = buildConf[i].name;
		}
		ui->buildConfigurationsPresetList->setItemText(i, presetName + (i == curPreset && presetModified ? "*" : ""));
	}
}

void CConfigDialog::updateKeyAssign(const QVector<Configuration::KeyAssignInfoType>& keyAssign)
{
	ui->keyAssignList->blockSignals(true);
	for(int i = 0; i < ui->keyAssignList->topLevelItemCount(); i++)
	{
		QTreeWidgetItem *item = ui->keyAssignList->topLevelItem(i);
		const Configuration::KeyAssignInfoType& sc = keyAssign[i];
		item->setData(KeyAssignListKeyAssignKeyColumn, Qt::DisplayRole, sc.keys);
	}
	ui->keyAssignList->blockSignals(false);
}

void CConfigDialog::applyKeyAssign(QVector<Configuration::KeyAssignInfoType>& keyAssign)
{
	keyAssign.clear();
	for(int i = 0, num = ui->keyAssignList->topLevelItemCount();
		i < num; i++)
	{
		QTreeWidgetItem *item = ui->keyAssignList->topLevelItem(i);
		Configuration::KeyAssignInfoType sc;
		sc.keys = QKeySequence(item->data(KeyAssignListKeyAssignKeyColumn, Qt::DisplayRole).toString());
		keyAssign.push_back(sc);
	}
}

void CConfigDialog::updateKeyAssignPresetList()
{
	ui->keyAssignPresetList->blockSignals(true);
	for(int i = ui->keyAssignPresetList->count();
		i < 1 + m_configuration.keyAssignPresetNum(); i++)
	{
		ui->keyAssignPresetList->addItem("");
	}
	for(int i = 1 + m_configuration.keyAssignPresetNum();
		i < ui->keyAssignPresetList->count(); i++)
	{
		ui->keyAssignPresetList->removeItem(ui->keyAssignPresetList->count() - 1);
	}
	ui->keyAssignPresetList->blockSignals(false);

	// 選択が無ければ「現在の設定」を選択
	int curPreset = ui->keyAssignPresetList->currentIndex() - 1;
	if( curPreset < 0 ) {
		ui->keyAssignPresetList->setCurrentIndex(0);
		curPreset = -1;
	}

	// 変更をチェック
	QVector<Configuration::KeyAssignInfoType> keyAssign;
	applyKeyAssign(keyAssign);
	bool presetModified = keyAssign != m_configuration.keyAssign(curPreset);

	ui->keyAssignPresetRemove->setDisabled(curPreset < 0 && !presetModified);

	QString presetName = tr("(Current setting)");
	for(int i = -1; i < m_configuration.keyAssignPresetNum(); i++)
	{
		if( 0 <= i )
		{
			presetName = m_configuration.keyAssignPresetName(i);
		}
		ui->keyAssignPresetList->setItemText(1 + i, presetName + (i == curPreset && presetModified ? "*" : ""));
	}
}

void CConfigDialog::updateToolbar(const QVector<Configuration::ActionEnum>& toolbar)
{
	ui->toolbarCurrentList->blockSignals(true);
	ui->toolbarCurrentList->clear();
	for(int i = 0; i < toolbar.size(); i++)
	{
		QListWidgetItem* item = new QListWidgetItem(ui->toolbarCurrentList);
		QListWidgetItem* itemBase = ui->toolbarPartsList->item((int)toolbar[i]);
		item->setText(itemBase->text());
		item->setIcon(itemBase->icon());
		item->setData(Qt::UserRole + 1, itemBase->data(Qt::UserRole + 1));
	}
	ui->toolbarCurrentList->setCurrentRow(0);
	ui->toolbarCurrentList->blockSignals(false);
}

void CConfigDialog::applyToolbar(QVector<Configuration::ActionEnum>& toolbar)
{
	toolbar.clear();
	for(int i = 0, num = ui->toolbarCurrentList->count();
		i < num; i++)
	{
		QListWidgetItem *item = ui->toolbarCurrentList->item(i);
		Configuration::ActionEnum action;
		action = Configuration::ActionEnum(item->data(Qt::UserRole + 1).toInt());
		toolbar.push_back(action);
	}
}

void CConfigDialog::updateToolbarPresetList()
{
	ui->toolbarPresetList->blockSignals(true);
	for(int i = ui->toolbarPresetList->count();
		i < 1 + m_configuration.toolbarPresetNum(); i++)
	{
		ui->toolbarPresetList->addItem("");
	}
	for(int i = 1 + m_configuration.toolbarPresetNum();
		i < ui->toolbarPresetList->count(); i++)
	{
		ui->toolbarPresetList->removeItem(ui->toolbarPresetList->count() - 1);
	}
	ui->toolbarPresetList->blockSignals(false);

	// 選択が無ければ「現在の設定」を選択
	int curPreset = ui->toolbarPresetList->currentIndex() - 1;
	if( curPreset < 0 ) {
		ui->toolbarPresetList->setCurrentIndex(0);
		curPreset = -1;
	}

	// 変更をチェック
	QVector<Configuration::ActionEnum> toolbar;
	applyToolbar(toolbar);
	bool presetModified = toolbar != m_configuration.toolbar(curPreset);

	ui->toolbarPresetRemove->setDisabled(curPreset < 0 && !presetModified);

	QString presetName = tr("(Current setting)");
	for(int i = -1; i < m_configuration.toolbarPresetNum(); i++)
	{
		if( 0 <= i )
		{
			presetName = m_configuration.toolbarPresetName(i);
		}
		ui->toolbarPresetList->setItemText(1 + i, presetName + (i == curPreset && presetModified ? "*" : ""));
	}
}

void CConfigDialog::checkUpdateToolbarPresetList()
{
	int curPreset = ui->toolbarPresetList->currentIndex() - 1;

	if( curPreset < -1 )
	{
		return;
	}

	QVector<Configuration::ActionEnum> toolbar;
	applyToolbar(toolbar);

	// プリセットに変更が加えられたら「現在の設定」と置き換える
	ui->toolbarPresetList->setCurrentIndex(0);

	updateToolbar(toolbar);

	updateToolbarPresetList();
}

void CConfigDialog::updateMetrics(int index)
{
    QTreeWidgetItem* item = ui->editorColorList->topLevelItem(index);
    QTreeWidgetItem* itemCur = ui->editorColorList->currentItem();
    if( !item ) {
        item = itemCur;
        if( !item ) {
            return;
        }
    }

    Configuration::ColorMetricsEnum indexOfMetrics = Configuration::ColorMetricsEnum(index);
    Configuration::ColorMetricsInfoType metrics;

    metrics.enable          = item->checkState(ColorListEnableColumn)   == Qt::Checked;
    metrics.useBoldFont     = item->checkState(ColorListFontBoldColumn) == Qt::Checked;
	metrics.foregroundColor = item->data(ColorListForegroundColorColumn, Qt::UserRole + 1).value<QColor>();
    metrics.backgroundColor = item->data(ColorListBackgroundColorColumn, Qt::UserRole + 1).value<QColor>();

    m_configuration.setColorMetrics(indexOfMetrics, metrics);

    QPixmap bmp(12, 12);

	ui->editorColorList->blockSignals(true);
    item->setCheckState(ColorListEnableColumn,               metrics.enable      ? Qt::Checked : Qt::Unchecked);
    item->setCheckState(ColorListFontBoldColumn,             metrics.useBoldFont ? Qt::Checked : Qt::Unchecked);
    item->setData(ColorListForegroundColorColumn, Qt::UserRole + 1, metrics.foregroundColor);
    item->setData(ColorListBackgroundColorColumn, Qt::UserRole + 1, metrics.backgroundColor);
    item->setIcon(ColorListForegroundColorColumn, (bmp.fill(metrics.foregroundColor), QIcon(bmp)));
    item->setIcon(ColorListBackgroundColorColumn, (bmp.fill(metrics.backgroundColor), QIcon(bmp)));
	ui->editorColorList->blockSignals(false);

    if( item == itemCur )
    {
        ui->editorColorItemEnable->setCheckState(metrics.enable      ? Qt::Checked : Qt::Unchecked);
        ui->editorColorItemBold->setCheckState(  metrics.useBoldFont ? Qt::Checked : Qt::Unchecked);

        bmp = QPixmap(16, 16);

        ui->editorColorItemFgcolor->setIcon((bmp.fill(metrics.foregroundColor), QIcon(bmp)));
        ui->editorColorItemBgcolor->setIcon((bmp.fill(metrics.backgroundColor), QIcon(bmp)));
    }
}

const Configuration& CConfigDialog::configuration() const
{
	return m_configuration;
}

void CConfigDialog::onOk()
{
	m_configuration.setHspPath(ui->installDirectoryPath->text());
	m_configuration.setHspCommonPath(ui->commonDirectoryPath->text());

	m_configuration.setEditorTabWidth(ui->editorTabWidth->value());
	m_configuration.setEditorLineNumberVisible(ui->editorEnableLineNumber->checkState() == Qt::Checked);
	m_configuration.setEditorLineNumberFontName(ui->editorLineNumberFont->currentFont().family());
	m_configuration.setEditorLineNumberFontSize(ui->editorLineNumberFontSize->value());
	m_configuration.setEditorFontName(ui->editorFont->currentFont().family());
	m_configuration.setEditorFontSize(ui->editorFontSize->value());

	Configuration::BuildConfList buildConf
		= m_configuration.buildConf();
	int curPreset = ui->buildConfigurationsPresetList->currentIndex();
	if( curPreset < buildConf.size() ) {
		applyBuildConf(buildConf[curPreset]);
		m_configuration.setBuildConf(buildConf);
	}

	QVector<Configuration::KeyAssignInfoType> keyAssign;
	applyKeyAssign(keyAssign);
	m_configuration.setKeyAssign(keyAssign);

	QVector<Configuration::ActionEnum> toolbar;
	applyToolbar(toolbar);
	m_configuration.setToolbar(toolbar);

	accept();
}

void CConfigDialog::onPageChanged(const QModelIndex& index)
{
	QStandardItem* item = static_cast<QStandardItem*>(index.internalPointer());
	               item = item->child(index.row(), 0);
	if( item->data().isValid() )
	{
		int oldPageIndex = ui->stackedWidget->currentIndex();
		int newPageIndex = item->data().toInt();
		// 変更破棄
		if( oldPageIndex != newPageIndex )
		{
			QWidget* currentPage = ui->stackedWidget->widget(oldPageIndex);
			if( ui->keyAssignPage == currentPage )
			{
				int curPreset = ui->keyAssignPresetList->currentIndex() - 1;
				updateKeyAssign(m_configuration.keyAssign(curPreset));
				updateKeyAssignPresetList();
			}
			else if( ui->toolbarPage == currentPage )
			{
				int curPreset = ui->toolbarPresetList->currentIndex() - 1;
				updateToolbar(m_configuration.toolbar(curPreset));
				updateToolbarPresetList();
			}
			else if( ui->buildConfigurationsPage == currentPage )
			{
				int curPreset = ui->buildConfigurationsPresetList->currentIndex();
				Configuration::BuildConfType buildConf
					= curPreset < m_configuration.buildConf().size() 
						? m_configuration.buildConf()[curPreset]
						: Configuration::BuildConfType();
				updateBuildConf(buildConf);
				updateBuildConfPresetList();
			}
		}
		// ページ切り替え
		ui->stackedWidget->setCurrentIndex(newPageIndex);
	}
	else
	{
		QModelIndex newIndex = item->child(0)->index();
		ui->category->setCurrentIndex(newIndex);
		onPageChanged(newIndex);
	}
}

void CConfigDialog::onClickedInstallDirectoryRef()
{
	QString path
		= QFileDialog::getExistingDirectory(this, QString(), ui->installDirectoryPath->text());
	if( !path.isEmpty() )
	{
		ui->installDirectoryPath->setText(QDir::toNativeSeparators(path));
	}
}

void CConfigDialog::onClickedCommonDirectoryRef()
{
	QString path
		= QFileDialog::getExistingDirectory(this, QString(), ui->commonDirectoryPath->text());
	if( !path.isEmpty() )
	{
		ui->commonDirectoryPath->setText(QDir::toNativeSeparators(path));
	}
}

void CConfigDialog::onCurrentMetricsChanged()
{
	QTreeWidgetItem* item = ui->editorColorList->currentItem();
	if( !item ) {
		return;
	}
    int index = ui->editorColorList->indexOfTopLevelItem(item);

    // henkoumae
    if( Configuration::ColorMetricsNum != m_lastIndexOfColorMetrics )
    {
        updateMetrics(m_lastIndexOfColorMetrics);
    }

    // henkougo
    if( m_lastIndexOfColorMetrics != Configuration::ColorMetricsEnum(index) )
    {
        m_lastIndexOfColorMetrics = Configuration::ColorMetricsEnum(index);

        // 表示更新＆状態保存
        updateMetrics(index);
    }
}

void CConfigDialog::onMetricsChanged(QTreeWidgetItem* item)
{
	if( !item ) {
		item = ui->editorColorList->currentItem();
		if( !item ) {
			return;
		}
	}

    // 表示更新＆状態保存
    int index = ui->editorColorList->indexOfTopLevelItem(item);
    updateMetrics(index);
}

void CConfigDialog::onChangedMetricsEnable(int state)
{
    QTreeWidgetItem* item = ui->editorColorList->currentItem();
    if( !item ) {
        return;
    }

    item->setCheckState(ColorListEnableColumn, state == Qt::Checked ? Qt::Checked : Qt::Unchecked);

    // 表示更新＆状態保存
    int index = ui->editorColorList->indexOfTopLevelItem(item);
    updateMetrics(index);
}

void CConfigDialog::onChangedMetricsFontBold(int state)
{
    QTreeWidgetItem* item = ui->editorColorList->currentItem();
    if( !item ) {
        return;
    }

    item->setCheckState(ColorListFontBoldColumn, state == Qt::Checked ? Qt::Checked : Qt::Unchecked);

    // 表示更新＆状態保存
    int index = ui->editorColorList->indexOfTopLevelItem(item);
    updateMetrics(index);
}

void CConfigDialog::onChangedMetricsBgcolor()
{
    QTreeWidgetItem* item = ui->editorColorList->currentItem();
    QAction* action = qobject_cast<QAction*>(sender());
	int index = action->data().toInt();

	if( 0 <= index )
	{
        item->setData(ColorListBackgroundColorColumn, Qt::UserRole + 1, QColor(Qt::GlobalColor(index)));
	}
	else
	{
		QColorDialog dlg;

        dlg.setCurrentColor(item->data(ColorListBackgroundColorColumn, Qt::UserRole + 1).value<QColor>());

		if( QDialog::Accepted == dlg.exec() )
		{
            item->setData(ColorListBackgroundColorColumn, Qt::UserRole + 1, dlg.selectedColor());
		}
	}

    // 表示更新＆状態保存
    int index_ = ui->editorColorList->indexOfTopLevelItem(item);
    updateMetrics(index_);
}

void CConfigDialog::onChangedMetricsFgcolor()
{
    QTreeWidgetItem* item = ui->editorColorList->currentItem();
    QAction* action = qobject_cast<QAction*>(sender());
	int index = action->data().toInt();

	if( 0 <= index )
	{
        item->setData(ColorListForegroundColorColumn, Qt::UserRole + 1, QColor(Qt::GlobalColor(index)));
	}
	else
	{
		QColorDialog dlg;

        dlg.setCurrentColor(item->data(ColorListForegroundColorColumn, Qt::UserRole + 1).value<QColor>());

        if( QDialog::Accepted == dlg.exec() )
        {
            item->setData(ColorListForegroundColorColumn, Qt::UserRole + 1, dlg.selectedColor());
        }
	}

    // 表示更新＆状態保存
    int index_ = ui->editorColorList->indexOfTopLevelItem(item);
    updateMetrics(index_);
}

void CConfigDialog::onBuildConfPresetChanged(int index)
{
	const Configuration::BuildConfList& buildConf = m_configuration.buildConf();
	Configuration::BuildConfType tmp = { QString() };
	updateBuildConf(0 <= index && index < buildConf.size() ? buildConf[index] : tmp);
	updateBuildConfPresetList();
}

void CConfigDialog::onBuildConfPresetRemove()
{
	Configuration::BuildConfList buildConf = m_configuration.buildConf();
	int index = ui->buildConfigurationsPresetList->currentIndex();

	if( 0 <= index && index < buildConf.size() )
	{
		buildConf.removeAt(index);
		m_configuration.setBuildConf(buildConf);
		index--;
		ui->buildConfigurationsPresetList->blockSignals(true);
		ui->buildConfigurationsPresetList->setCurrentIndex(index);
		ui->buildConfigurationsPresetList->blockSignals(false);
	}
	if( 0 <= index && index < buildConf.size() )
	{
		updateBuildConf(buildConf[index]);
	}

	updateBuildConfPresetList();
}

void CConfigDialog::onBuildConfPresetSave()
{
	CSavePresetDialog dlg(this);

	Configuration::BuildConfList buildConf = m_configuration.buildConf();
	int curPreset = ui->buildConfigurationsPresetList->currentIndex();
	bool presetAppend = buildConf.size() <= curPreset;
	dlg.setNewTitlePresent(presetAppend);
	dlg.setNewTitle(presetAppend ? "" : buildConf[curPreset].name);

	if( QDialog::Accepted != dlg.exec() )
	{
		return;
	}

	int newSelectPreset = -1;

	if( dlg.newTitlePresent() )
	{
		// 別名でプリセットを追加
		buildConf.push_back(Configuration::BuildConfType());
		applyBuildConf(buildConf.back());
		buildConf.back().name = dlg.newTitle();
		m_configuration.setBuildConf(buildConf);
		newSelectPreset = buildConf.size() - 1;
	}
	else
	{
		// 上書き保存
		applyBuildConf(buildConf[curPreset]);
		m_configuration.setBuildConf(buildConf);
		newSelectPreset = ui->buildConfigurationsPresetList->currentIndex();
	}

	updateBuildConfPresetList();
	// 追加したプリセットに変更
	ui->buildConfigurationsPresetList->setCurrentIndex(newSelectPreset);
}

void CConfigDialog::onBuildConfModified()
{
	updateBuildConfPresetList();
}

void CConfigDialog::onKeyAssignPresetChanged(int index)
{
	updateKeyAssign(m_configuration.keyAssign(index - 1));
	updateKeyAssignPresetList();
}

void CConfigDialog::onKeyAssignPresetRemove()
{
	int curPreset = ui->keyAssignPresetList->currentIndex() - 1;

	if( curPreset < 0 )
	{
		updateKeyAssign(m_configuration.keyAssign());
	}
	else
	{
		m_configuration.removeKeyAssignPreset(curPreset);
	}

	updateKeyAssignPresetList();
}

void CConfigDialog::onKeyAssignChanged(QTreeWidgetItem* /*item*/, int column)
{
	int curPreset = ui->keyAssignPresetList->currentIndex() - 1;

	if( KeyAssignListKeyAssignKeyColumn != column ||
		curPreset < -1 )
	{
		return;
	}

	QVector<Configuration::KeyAssignInfoType> keyAssign;
	applyKeyAssign(keyAssign);

	// プリセットに変更が加えられたら「現在の設定」と置き換える
	ui->keyAssignPresetList->setCurrentIndex(0);

	updateKeyAssign(keyAssign);

	updateKeyAssignPresetList();
}

void CConfigDialog::onKeyAssignPresetSave()
{
	CSavePresetDialog dlg(this);

	int curPreset = ui->keyAssignPresetList->currentIndex() - 1;
	bool presetCurrent = curPreset < 0;
	dlg.setNewTitlePresent(presetCurrent);
	dlg.setNewTitle(presetCurrent ? "" : m_configuration.keyAssignPresetName(curPreset));

	if( QDialog::Accepted != dlg.exec() )
	{
		return;
	}

	int newSelectPreset = -1;

	if( dlg.newTitlePresent() )
	{
		// 別名でプリセットを追加
		QVector<Configuration::KeyAssignInfoType> keyAssign;
		applyKeyAssign(keyAssign);
		m_configuration.appendKeyAssignPreset(dlg.newTitle(), keyAssign);
		newSelectPreset = m_configuration.keyAssignPresetNum();
	}
	else
	{
		// 上書き保存
		QVector<Configuration::KeyAssignInfoType> keyAssign;
		applyKeyAssign(keyAssign);
		m_configuration.setKeyAssign(
			ui->keyAssignPresetList->currentIndex() - 1,
			keyAssign);
		newSelectPreset = ui->keyAssignPresetList->currentIndex();
	}

	updateKeyAssignPresetList();
	// 追加したプリセットに変更
	ui->keyAssignPresetList->setCurrentIndex(newSelectPreset);
}

void CConfigDialog::onToolbarPresetChanged(int index)
{
	updateToolbar(m_configuration.toolbar(index - 1));
	updateToolbarPresetList();
}

void CConfigDialog::onToolbarPresetSave()
{
	CSavePresetDialog dlg(this);

	int curPreset = ui->toolbarPresetList->currentIndex() - 1;
	bool presetCurrent = curPreset < 0;
	dlg.setNewTitlePresent(presetCurrent);
	dlg.setNewTitle(presetCurrent ? "" : m_configuration.toolbarPresetName(curPreset));

	if( QDialog::Accepted != dlg.exec() )
	{
		return;
	}

	int newSelectPreset = -1;

	if( dlg.newTitlePresent() )
	{
		// 別名でプリセットを追加
		QVector<Configuration::ActionEnum> toolbar;
		applyToolbar(toolbar);
		m_configuration.appendToolbarPreset(dlg.newTitle(), toolbar);
		newSelectPreset = m_configuration.toolbarPresetNum();
	}
	else
	{
		// 上書き保存
		QVector<Configuration::ActionEnum> toolbar;
		applyToolbar(toolbar);
		m_configuration.setToolbar(
			ui->toolbarPresetList->currentIndex() - 1,
			toolbar);
		newSelectPreset = ui->toolbarPresetList->currentIndex();
	}

	updateToolbarPresetList();
	// 追加したプリセットに変更
	ui->toolbarPresetList->setCurrentIndex(newSelectPreset);
}

void CConfigDialog::onToolbarPresetRemove()
{
	int curPreset = ui->toolbarPresetList->currentIndex() - 1;

	if( curPreset < 0 )
	{
		updateToolbar(m_configuration.toolbar());
	}
	else
	{
		m_configuration.removeToolbarPreset(curPreset);
	}

	updateToolbarPresetList();
}

void CConfigDialog::onToolbarAppendButton()
{
	Configuration::ActionEnum action
		= Configuration::ActionEnum(ui->toolbarPartsList->currentRow());
	int row = ui->toolbarCurrentList->currentRow();
	QListWidgetItem* item     = new QListWidgetItem();
	QListWidgetItem* itemBase = ui->toolbarPartsList->item((int)action);
	item->setText(itemBase->text());
	item->setIcon(itemBase->icon());
	item->setData(Qt::UserRole + 1, itemBase->data(Qt::UserRole + 1));
	ui->toolbarCurrentList->insertItem(row, item);
	row = ui->toolbarCurrentList->row(item);
	checkUpdateToolbarPresetList();
	ui->toolbarCurrentList->setCurrentRow(row);
}

void CConfigDialog::onToolbarAppendButtonLast()
{
	Configuration::ActionEnum action
		= Configuration::ActionEnum(ui->toolbarPartsList->currentRow());
	QListWidgetItem* item     = new QListWidgetItem();
	QListWidgetItem* itemBase = ui->toolbarPartsList->item((int)action);
	item->setText(itemBase->text());
	item->setIcon(itemBase->icon());
	item->setData(Qt::UserRole + 1, itemBase->data(Qt::UserRole + 1));
	ui->toolbarCurrentList->addItem(item);
	int row = ui->toolbarCurrentList->row(item);
	checkUpdateToolbarPresetList();
	ui->toolbarCurrentList->setCurrentRow(row);
}

void CConfigDialog::onToolbarRemoveButton()
{
	int row = ui->toolbarCurrentList->currentRow();
	ui->toolbarCurrentList->takeItem(row);
	row = ui->toolbarCurrentList->currentRow();
	checkUpdateToolbarPresetList();
	ui->toolbarCurrentList->setCurrentRow(row);
}

void CConfigDialog::onToolbarButtonGoUp()
{
	int rowOld = ui->toolbarCurrentList->currentRow();
	int rowNew = (std::max)(0, rowOld - 1);
	QListWidgetItem* item = ui->toolbarCurrentList->takeItem(rowOld);
	ui->toolbarCurrentList->insertItem(rowNew, item);
	checkUpdateToolbarPresetList();
	ui->toolbarCurrentList->setCurrentRow(rowNew);
}

void CConfigDialog::onToolbarButtonGoDown()
{
	int rowOld = ui->toolbarCurrentList->currentRow();
	int rowNew = (std::min)(ui->toolbarCurrentList->count() - 1, rowOld + 1);
	QListWidgetItem* item = ui->toolbarCurrentList->takeItem(rowOld);
	ui->toolbarCurrentList->insertItem(rowNew, item);
	checkUpdateToolbarPresetList();
	ui->toolbarCurrentList->setCurrentRow(rowNew);
}
