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
	, m_lastIndexOfColorMetrics(Configuration::ColorMetricsNum)
	, m_blockUpdateKeyAssign(false)
{
	ui.setupUi(this);

	QStandardItemModel* model;
	QStandardItem* rootItem;
	QStandardItem* item, *item2;
	QItemEditorFactory* factory;
	QItemDelegate* itemDelegate;

	// 設定画面のページ一覧を登録
	ui.category->setRootIsDecorated(false);
	ui.category->setHeaderHidden(true);
	ui.category->setModel(model = new QStandardItemModel());
	rootItem = model->invisibleRootItem();
	rootItem->appendRow(item = new QStandardItem(tr("Directory")));
		item->setData(ui.stackedWidget->indexOf(ui.directoryPage));
	rootItem->appendRow(item = new QStandardItem(tr("Editor")));
		item->appendRow(item2 = new QStandardItem(tr("General")));
			item2->setData(ui.stackedWidget->indexOf(ui.editorGeneralPage));
		item->appendRow(item2 = new QStandardItem(tr("Color")));
			item2->setData(ui.stackedWidget->indexOf(ui.editorColorPage));
	rootItem->appendRow(item = new QStandardItem(tr("Tools")));
		item->setData(ui.stackedWidget->indexOf(ui.toolsPage));
	rootItem->appendRow(item = new QStandardItem(tr("Key assign")));
		item->setData(ui.stackedWidget->indexOf(ui.keyAssignPage));
	rootItem->appendRow(item = new QStandardItem(tr("Toolbar")));
		item->setData(ui.stackedWidget->indexOf(ui.toolbarPage));
	ui.category->expandAll();

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
	ui.editorColorItemBgcolor->setMenu(menu1);
	ui.editorColorItemFgcolor->setMenu(menu2);

	// 色設定の一覧を初期化
	ui.editorColorList->setColumnCount(ColorListColumnNum);
	ui.editorColorList->header()->setResizeMode(ColorListCategoryColumn,        QHeaderView::Stretch);
	ui.editorColorList->header()->setResizeMode(ColorListEnableColumn,          QHeaderView::ResizeToContents);
	ui.editorColorList->header()->setResizeMode(ColorListFontBoldColumn,        QHeaderView::ResizeToContents);
	ui.editorColorList->header()->setResizeMode(ColorListForegroundColorCount,  QHeaderView::ResizeToContents);
	ui.editorColorList->header()->setResizeMode(ColorListBackgroundColorColumn, QHeaderView::ResizeToContents);
	ui.editorColorList->setItemDelegate(itemDelegate = new CExpandedItemHeightDelegate);

	// ショートカットの一覧を初期化
	ui.keyAssignList->setColumnCount(KeyAssignListColumnNum);
	ui.keyAssignList->header()->setResizeMode(KeyAssignListNameColumn,        QHeaderView::Stretch);
	ui.keyAssignList->header()->setResizeMode(KeyAssignListKeyAssignKeyColumn, QHeaderView::ResizeToContents);
	ui.keyAssignList->setItemDelegate(itemDelegate = new CKeyAssignListItemDelegate);
		itemDelegate->setItemEditorFactory(factory = new QItemEditorFactory);
			factory->registerEditor(QVariant::KeySequence, new QStandardItemEditorCreator<CHotkeyWidget>());

	ui.category->selectionModel()->clear();
	ui.category->selectionModel()->select(rootItem->child(0)->index(), QItemSelectionModel::Select|QItemSelectionModel::Current);
	ui.stackedWidget->setCurrentIndex(0);

	setConfiguration(theConf);
}

void CConfigDialog::setConfiguration(const Configuration& info)
{
	m_configuration = info;

	// 「ディレクトリ」ページ初期化

	ui.installDirectoryPath->setText(m_configuration.hspPath());
	ui.commonDirectoryPath->setText(m_configuration.hspCommonPath());

	// 「エディタ - 一般」ページ初期化

	ui.editorTabWidth->setValue(m_configuration.editorTabWidth());
	ui.editorEnableLineNumber->setCheckState(m_configuration.editorLineNumberVisibled() ? Qt::Checked : Qt::Unchecked);
	ui.editorLineNumberFont->setCurrentFont(QFont(m_configuration.editorLineNumberFontName()));
	ui.editorLineNumberFontSize->setValue(m_configuration.editorLineNumberFontSize());
	ui.editorFont->setCurrentFont(QFont(m_configuration.editorFontName()));
	ui.editorFontSize->setValue(m_configuration.editorFontSize());

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
				<< tr("String")
				<< tr("Line feed char")
				<< tr("Tab char")
				<< tr("End of file")
				;
	ui.editorColorList->clear();
	for(int i = 0; i < categoryList.size(); i++)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(ui.editorColorList);
		const Configuration::ColorMetricsInfoType& metricsInfo = m_configuration.colorMetrics(Configuration::ColorMetricsEnum(i));
		item->setText(ColorListCategoryColumn,                   categoryList.at(i));
		item->setCheckState(ColorListEnableColumn,               metricsInfo.enable      ? Qt::Checked : Qt::Unchecked);
		item->setCheckState(ColorListFontBoldColumn,             metricsInfo.useBoldFont ? Qt::Checked : Qt::Unchecked);
		item->setIcon(ColorListForegroundColorCount,  (bmp.fill(metricsInfo.foregroundColor), QIcon(bmp)));
		item->setIcon(ColorListBackgroundColorColumn, (bmp.fill(metricsInfo.backgroundColor), QIcon(bmp)));
	}
	ui.editorColorList->resizeColumnToContents(ColorListEnableColumn);
	ui.editorColorList->resizeColumnToContents(ColorListFontBoldColumn);
	ui.editorColorList->setCurrentItem(ui.editorColorList->topLevelItem(0));

	// 「キー割り当て」ページ初期化

	QStringList scValues;
	scValues	<< tr("New")
				<< tr("Open")
				<< tr("Save")
				<< tr("Save as")
				<< tr("Save all")
				<< tr("Quit")
				<< tr("Undo")
				<< tr("Redo")
				<< tr("Cut")
				<< tr("Copy")
				<< tr("Paste")
				<< tr("Clear")
				<< tr("Select all")
				<< tr("Find")
				<< tr("Find next")
				<< tr("Find previous")
				<< tr("Replace")
				<< tr("Jump")
				<< tr("Build solution")
				<< tr("Build project")
				<< tr("Compile only")
				<< tr("Batch build")
				<< tr("Debug run solution")
				<< tr("No debug run solution")
				<< tr("Debug run project")
				<< tr("No debug run project")
				<< tr("Debug suspend")
				<< tr("Debug resume")
				<< tr("Debug stop")
				<< tr("Configuration")
				<< tr("Show project")
				<< tr("Show symbols")
				<< tr("Show output")
				<< tr("Show search")
				<< tr("Show messages")
				<< tr("Show breakpoints")
				<< tr("Show system variables")
				<< tr("Show variables")
				;
	ui.keyAssignList->clear();
	for(int i = 0; i < scValues.size(); i++)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(ui.keyAssignList);
		item->setText(KeyAssignListNameColumn, scValues[i]);
		item->setFlags(item->flags()|Qt::ItemIsEditable);
	}
	updateKeyAssign(m_configuration.keyAssign());

	updateKeyAssignPresetList();

	//
}

void CConfigDialog::updateKeyAssign(const QVector<Configuration::KeyAssignInfoType>& keyAssign)
{
	m_blockUpdateKeyAssign = true;
	for(int i = 0; i < ui.keyAssignList->topLevelItemCount(); i++)
	{
		QTreeWidgetItem *item = ui.keyAssignList->topLevelItem(i);
		const Configuration::KeyAssignInfoType& sc = keyAssign[i];
		item->setData(KeyAssignListKeyAssignKeyColumn, Qt::DisplayRole, sc.keys);
	}
	m_blockUpdateKeyAssign = false;
}

void CConfigDialog::applyKeyAssign(QVector<Configuration::KeyAssignInfoType>& keyAssign)
{
	keyAssign.resize(ui.keyAssignList->topLevelItemCount());
	for(int i = 0; i < ui.keyAssignList->topLevelItemCount(); i++)
	{
		QTreeWidgetItem *item = ui.keyAssignList->topLevelItem(i);
		Configuration::KeyAssignInfoType& sc = keyAssign[i];
		sc.keys = QKeySequence(item->data(KeyAssignListKeyAssignKeyColumn, Qt::DisplayRole).toString());
	}
}

void CConfigDialog::updateKeyAssignPresetList()
{
	for(int i = ui.keyAssignPresetList->count();
		i < 1 + m_configuration.keyAssignPresetNum(); i++)
	{
		ui.keyAssignPresetList->addItem("");
	}
	for(int i = 1 + m_configuration.keyAssignPresetNum();
		i < ui.keyAssignPresetList->count(); i++)
	{
		ui.keyAssignPresetList->removeItem(ui.keyAssignPresetList->count() - 1);
	}

	// 選択が無ければ「現在の設定」を選択
	int curPreset = ui.keyAssignPresetList->currentIndex() - 1;
	if( curPreset < 0 ) {
		ui.keyAssignPresetList->setCurrentIndex(0);
		curPreset = -1;
	}

	// 変更をチェック
	QVector<Configuration::KeyAssignInfoType> keyAssign;
	applyKeyAssign(keyAssign);
	bool presetModified = keyAssign != m_configuration.keyAssign(curPreset);

	ui.keyAssignPresetRemove->setDisabled(curPreset < 0 && !presetModified);

	QString presetName = tr("Current setting");
	for(int i = -1; i < m_configuration.keyAssignPresetNum(); i++)
	{
		if( 0 <= i )
		{
			presetName = m_configuration.keyAssignPresetName(i);
		}
		ui.keyAssignPresetList->setItemText(1 + i, presetName + (i == curPreset && presetModified ? "*" : ""));
	}
}

const Configuration& CConfigDialog::configuration() const
{
	return m_configuration;
}

void CConfigDialog::onOk()
{
	m_configuration.setHspPath(ui.installDirectoryPath->text());
	m_configuration.setHspCommonPath(ui.commonDirectoryPath->text());

	m_configuration.setEditorTabWidth(ui.editorTabWidth->value());
	m_configuration.setEditorLineNumberVisible(ui.editorEnableLineNumber->checkState() == Qt::Checked);
	m_configuration.setEditorLineNumberFontName(ui.editorLineNumberFont->currentFont().family());
	m_configuration.setEditorLineNumberFontSize(ui.editorLineNumberFontSize->value());
	m_configuration.setEditorFontName(ui.editorFont->currentFont().family());
	m_configuration.setEditorFontSize(ui.editorFontSize->value());

	QVector<Configuration::KeyAssignInfoType> keyAssign;
	applyKeyAssign(keyAssign);
	m_configuration.setKeyAssign(keyAssign);

	accept();
}

void CConfigDialog::onPageChanged(const QModelIndex& index)
{
	QStandardItem* item = static_cast<QStandardItem*>(index.internalPointer());
	               item = item->child(index.row(), 0);
	if( item->data().isValid() )
	{
		// 変更破棄
		if( ui.keyAssignPage == ui.stackedWidget->widget(ui.stackedWidget->currentIndex()) )
		{
			int curPreset = ui.keyAssignPresetList->currentIndex() - 1;
			updateKeyAssign(m_configuration.keyAssign(curPreset));
		}
		// ページ切り替え
		ui.stackedWidget->setCurrentIndex(item->data().toInt());
	}
	else
	{
		QModelIndex newIndex = item->child(0)->index();
		ui.category->setCurrentIndex(newIndex);
		onPageChanged(newIndex);
	}
}

void CConfigDialog::onClickedInstallDirectoryRef()
{
	QString path
		= QFileDialog::getExistingDirectory(this, QString(), ui.installDirectoryPath->text());
	if( !path.isEmpty() )
	{
		ui.installDirectoryPath->setText(QDir::toNativeSeparators(path));
	}
}

void CConfigDialog::onClickedCommonDirectoryRef()
{
	QString path
		= QFileDialog::getExistingDirectory(this, QString(), ui.commonDirectoryPath->text());
	if( !path.isEmpty() )
	{
		ui.commonDirectoryPath->setText(QDir::toNativeSeparators(path));
	}
}

void CConfigDialog::onCurrentMetricsChanged()
{
	QTreeWidgetItem* item = ui.editorColorList->currentItem();
	if( !item ) {
		return;
	}

	int index = ui.editorColorList->indexOfTopLevelItem(item);
	Configuration::ColorMetricsEnum metrics = Configuration::ColorMetricsEnum(index);

	if( Configuration::ColorMetricsNum != m_lastIndexOfColorMetrics )
	{
		m_configuration.setColorMetrics(m_lastIndexOfColorMetrics, m_currentColorMetricsInfo);
	}

	m_lastIndexOfColorMetrics = metrics;
	m_currentColorMetricsInfo = m_configuration.colorMetrics(metrics);

	QPixmap bmp(12, 12);

	item->setCheckState(ColorListEnableColumn,               m_currentColorMetricsInfo.enable      ? Qt::Checked : Qt::Unchecked);
	item->setCheckState(ColorListFontBoldColumn,             m_currentColorMetricsInfo.useBoldFont ? Qt::Checked : Qt::Unchecked);
	item->setIcon(ColorListForegroundColorCount,  (bmp.fill(m_currentColorMetricsInfo.foregroundColor), QIcon(bmp)));
	item->setIcon(ColorListBackgroundColorColumn, (bmp.fill(m_currentColorMetricsInfo.backgroundColor), QIcon(bmp)));

	ui.editorColorItemEnable->setCheckState(m_currentColorMetricsInfo.enable      ? Qt::Checked : Qt::Unchecked);
	ui.editorColorItemBold->setCheckState(  m_currentColorMetricsInfo.useBoldFont ? Qt::Checked : Qt::Unchecked);

	bmp = QPixmap(16, 16);

	ui.editorColorItemFgcolor->setIcon((bmp.fill(m_currentColorMetricsInfo.foregroundColor), QIcon(bmp)));
	ui.editorColorItemBgcolor->setIcon((bmp.fill(m_currentColorMetricsInfo.backgroundColor), QIcon(bmp)));
}

void CConfigDialog::onMetricsChanged(QTreeWidgetItem* item)
{
	if( !item ) {
		item = ui.editorColorList->currentItem();
		if( !item ) {
			return;
		}
	}

	bool update = false;

	update |= m_currentColorMetricsInfo.enable      != (item->checkState(ColorListEnableColumn)   == Qt::Checked);
	update |= m_currentColorMetricsInfo.useBoldFont != (item->checkState(ColorListFontBoldColumn) == Qt::Checked);
	m_currentColorMetricsInfo.enable      = item->checkState(ColorListEnableColumn)   == Qt::Checked;
	m_currentColorMetricsInfo.useBoldFont = item->checkState(ColorListFontBoldColumn) == Qt::Checked;

	if( update ) {
		onCurrentMetricsChanged();
	}
}

void CConfigDialog::onChangedMetricsEnable(int state)
{
	m_currentColorMetricsInfo.enable = state == Qt::Checked;

	onCurrentMetricsChanged();
}

void CConfigDialog::onChangedMetricsFontBold(int state)
{
	m_currentColorMetricsInfo.useBoldFont = state == Qt::Checked;

	onCurrentMetricsChanged();
}

void CConfigDialog::onChangedMetricsBgcolor()
{
	QAction* action = qobject_cast<QAction*>(sender());
	int index = action->data().toInt();

	if( 0 <= index )
	{
		m_currentColorMetricsInfo.backgroundColor = Qt::GlobalColor(index);
	}
	else
	{
		QColorDialog dlg;

		dlg.setCurrentColor(m_currentColorMetricsInfo.backgroundColor);

		if( QDialog::Accepted == dlg.exec() )
		{
			m_currentColorMetricsInfo.backgroundColor = dlg.selectedColor();
		}
	}

	onCurrentMetricsChanged();
}

void CConfigDialog::onChangedMetricsFgcolor()
{
	QAction* action = qobject_cast<QAction*>(sender());
	int index = action->data().toInt();

	if( 0 <= index )
	{
		m_currentColorMetricsInfo.foregroundColor = Qt::GlobalColor(index);
	}
	else
	{
		QColorDialog dlg;

		dlg.setCurrentColor(m_currentColorMetricsInfo.foregroundColor);

		if( QDialog::Accepted == dlg.exec() )
		{
			m_currentColorMetricsInfo.foregroundColor = dlg.selectedColor();
		}
	}

	onCurrentMetricsChanged();
}

void CConfigDialog::onKeyAsignPresetChanged(int index)
{
	updateKeyAssign(m_configuration.keyAssign(index - 1));
	updateKeyAssignPresetList();
}

void CConfigDialog::onKeyAsignPresetRemove()
{
	int curPreset = ui.keyAssignPresetList->currentIndex() - 1;

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

void CConfigDialog::onKeyAssignChanged(QTreeWidgetItem* item, int column)
{
	int curPreset = ui.keyAssignPresetList->currentIndex() - 1;

	if( KeyAssignListKeyAssignKeyColumn != column ||
		curPreset < -1 ||
		m_blockUpdateKeyAssign )
	{
		return;
	}

	const QVector<Configuration::KeyAssignInfoType>& keyAssignBase = m_configuration.keyAssign(curPreset);
	QVector<Configuration::KeyAssignInfoType> keyAssign;
	applyKeyAssign(keyAssign);

	// プリセットに変更が加えられたら「現在の設定」と置き換える
	ui.keyAssignPresetList->setCurrentIndex(0);

	updateKeyAssign(keyAssign);

	updateKeyAssignPresetList();
}

void CConfigDialog::onKeyAsignPresetSave()
{
	CSavePresetDialog dlg(this);

	int curPreset = ui.keyAssignPresetList->currentIndex() - 1;
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
			ui.keyAssignPresetList->currentIndex() - 1,
			keyAssign);
		newSelectPreset = ui.keyAssignPresetList->currentIndex();
	}

	updateKeyAssignPresetList();
	// 追加したプリセットに変更
	ui.keyAssignPresetList->setCurrentIndex(newSelectPreset);
}

void CConfigDialog::onToolbarPresetChanged(int index)
{
}

void CConfigDialog::onToolbarPresetSave()
{
}

void CConfigDialog::onToolbarPresetRemove()
{
}

void CConfigDialog::onToolbarAppendButton()
{
}
void CConfigDialog::onToolbarAppendSeparator()
{
}
void CConfigDialog::onToolbarRemoveButton()
{
}
void CConfigDialog::onToolbarButtonGoTop()
{
}
void CConfigDialog::onToolbarButtonGoBottom()
{
}
