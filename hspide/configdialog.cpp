#include "configdialog.h"
#include <QtGui>

CConfigDialog::CConfigDialog(QWidget *parent)
	: QDialog(parent)
	, m_lastIndexOfColorMetrics(Configuration::ColorMetricsNum)
{
	ui.setupUi(this);

	QStandardItemModel* model;
	QStandardItem* rootItem;
	QStandardItem* item, *item2;
	// Ý’è‰æ–Ê‚Ìƒy[ƒWˆê——‚ð“o˜^
	ui.category->setRootIsDecorated(false);
	ui.category->setHeaderHidden(true);
	ui.category->setModel(model = new QStandardItemModel());
	ui.category->setEditTriggers(QTreeView::NoEditTriggers);
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
	rootItem->appendRow(item = new QStandardItem(tr("Shortcut key")));
		item->setData(ui.stackedWidget->indexOf(ui.shortcutKeyPage));
	ui.category->expandAll();

	QStringList colorNames;
	colorNames	<< tr("White")
				<< tr("Black")
				<< tr("Red")
				<< tr("Dark red")
				<< tr("Green")
				<< tr("Dark green")
				<< tr("Blue")
				<< tr("Dark blue")
				<< tr("Cyan")
				<< tr("Dark cyan")
				<< tr("Magenta")
				<< tr("Dark magenta")
				<< tr("Yellow")
				<< tr("Dark yellow")
				<< tr("Gray")
				<< tr("Dark gray")
				<< tr("Light gray")
				;
	QList<Qt::GlobalColor> colorValues;
	colorValues	<< Qt::white
				<< Qt::black
				<< Qt::red
				<< Qt::darkRed
				<< Qt::green
				<< Qt::darkGreen
				<< Qt::blue
				<< Qt::darkBlue
				<< Qt::cyan
				<< Qt::darkCyan
				<< Qt::magenta
				<< Qt::darkMagenta
				<< Qt::yellow
				<< Qt::darkYellow
				<< Qt::gray
				<< Qt::darkGray
				<< Qt::lightGray
				;
	QMenu* menu1 = new QMenu(this);
	QMenu* menu2 = new QMenu(this);
	QAction* action1, *action2;
		for(int i = 0; i < colorNames.size(); i++)
		{
			QPixmap bmp(16, 16);
			bmp.fill(colorValues.at(i));
			action1 = menu1->addAction(QIcon(bmp), colorNames.at(i));
			action2 = menu2->addAction(QIcon(bmp), colorNames.at(i));
			action1->setData(int(colorValues.at(i)));
			action2->setData(int(colorValues.at(i)));
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

	// Ý’è‰æ–Ê‚Ìƒy[ƒWˆê——‚ð“o˜^
	ui.editorColorList->setEditTriggers(QTreeView::NoEditTriggers);
	ui.editorColorList->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.editorColorList->setColumnCount(ColumnNum);
	ui.editorColorList->header()->setResizeMode(CategoryColumn,        QHeaderView::Stretch);
	ui.editorColorList->header()->setResizeMode(EnableColumn,          QHeaderView::ResizeToContents);
	ui.editorColorList->header()->setResizeMode(FontBoldColumn,        QHeaderView::ResizeToContents);
	ui.editorColorList->header()->setResizeMode(ForegroundColorCount,  QHeaderView::ResizeToContents);
	ui.editorColorList->header()->setResizeMode(BackgroundColorColumn, QHeaderView::ResizeToContents);

	ui.category->selectionModel()->clear();
	ui.category->selectionModel()->select(rootItem->child(0)->index(), QItemSelectionModel::Select|QItemSelectionModel::Current);
	ui.stackedWidget->setCurrentIndex(0);
}

void CConfigDialog::setConfiguration(const Configuration& info)
{
	m_configuration = info;

	ui.installDirectoryPath->setText(m_configuration.hspPath());
	ui.commonDirectoryPath->setText(m_configuration.hspCommonPath());

	ui.editorTabWidth->setValue(m_configuration.editorTabWidth());
	ui.editorEnableLineNumber->setCheckState(m_configuration.editorLineNumberVisibled() ? Qt::Checked : Qt::Unchecked);
	ui.editorLineNumberFont->setCurrentFont(QFont(m_configuration.editorLineNumberFontName()));
	ui.editorLineNumberFontSize->setValue(m_configuration.editorLineNumberFontSize());
	ui.editorFont->setCurrentFont(QFont(m_configuration.editorFontName()));
	ui.editorFontSize->setValue(m_configuration.editorFontSize());

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
		item->setText(CategoryColumn,                   categoryList.at(i));
		item->setCheckState(EnableColumn,               metricsInfo.enable      ? Qt::Checked : Qt::Unchecked);
		item->setCheckState(FontBoldColumn,             metricsInfo.useBoldFont ? Qt::Checked : Qt::Unchecked);
		item->setIcon(ForegroundColorCount,  (bmp.fill(metricsInfo.foregroundColor), QIcon(bmp)));
		item->setIcon(BackgroundColorColumn, (bmp.fill(metricsInfo.backgroundColor), QIcon(bmp)));
	}
	ui.editorColorList->resizeColumnToContents(EnableColumn);
	ui.editorColorList->resizeColumnToContents(FontBoldColumn);
	ui.editorColorList->setCurrentItem(ui.editorColorList->topLevelItem(0));
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

	accept();
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

	item->setCheckState(EnableColumn,               m_currentColorMetricsInfo.enable      ? Qt::Checked : Qt::Unchecked);
	item->setCheckState(FontBoldColumn,             m_currentColorMetricsInfo.useBoldFont ? Qt::Checked : Qt::Unchecked);
	item->setIcon(ForegroundColorCount,  (bmp.fill(m_currentColorMetricsInfo.foregroundColor), QIcon(bmp)));
	item->setIcon(BackgroundColorColumn, (bmp.fill(m_currentColorMetricsInfo.backgroundColor), QIcon(bmp)));

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

	update |= m_currentColorMetricsInfo.enable      != (item->checkState(EnableColumn)   == Qt::Checked);
	update |= m_currentColorMetricsInfo.useBoldFont != (item->checkState(FontBoldColumn) == Qt::Checked);
	m_currentColorMetricsInfo.enable      = item->checkState(EnableColumn)   == Qt::Checked;
	m_currentColorMetricsInfo.useBoldFont = item->checkState(FontBoldColumn) == Qt::Checked;

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

		if( QDialog::Accepted == dlg.exec() )
		{
			m_currentColorMetricsInfo.foregroundColor = dlg.selectedColor();
		}
	}

	onCurrentMetricsChanged();
}
