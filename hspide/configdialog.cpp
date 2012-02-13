#include "configdialog.h"
#include <QtGui>

CConfigDialog::CConfigDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	QStandardItemModel* model;
	QStandardItem* rootItem;
	QStandardItem* item, *item2;
	// 設定画面のページ一覧を登録
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

	// 設定画面のページ一覧を登録
	QStringList categoryList;
	categoryList<< tr("Line number")
				<< tr("Function")
				<< tr("Sub routine")
				<< tr("Preprocessor")
				<< tr("Macro")
				<< tr("Line feed char")
				<< tr("Tab char")
				<< tr("End of file")
				;
	ui.editorColorList->setRowCount(categoryList.size());
	ui.editorColorList->setColumnCount(ColumnNum);
	ui.editorColorList->setColumnWidth(EnableColumn,   20);
	ui.editorColorList->setColumnWidth(FontBoldColumn, 20);
	ui.editorColorList->setHorizontalHeaderLabels(
			QStringList()
				<< tr("Enable")
				<< tr("Category")
				<< tr("FontBold")
				<< tr("BackgroundColor")
				<< tr("ForegroundColor")
		);
    ui.editorColorList->horizontalHeader()->setStretchLastSection(true);
    ui.editorColorList->verticalHeader()->setVisible(false);
	for(int i = 0; i < categoryList.size(); i++)
	{
		QString category = categoryList.at(i);
		QTableWidgetItem *enableItem, *categoryItem, *fontBoldItem, *backgroundColorItem, *foregroundColorItem;
		ui.editorColorList->setItem(i, EnableColumn,          enableItem          = new QTableWidgetItem());
		ui.editorColorList->setItem(i, CategoryColumn,        categoryItem        = new QTableWidgetItem(category));
		ui.editorColorList->setItem(i, FontBoldColumn,        fontBoldItem        = new QTableWidgetItem());
		ui.editorColorList->setItem(i, BackgroundColorColumn, backgroundColorItem = new QTableWidgetItem());
		ui.editorColorList->setItem(i, ForegroundColorCount,  foregroundColorItem = new QTableWidgetItem());
		enableItem->setCheckState(Qt::Checked);
		fontBoldItem->setCheckState(Qt::Checked);
		backgroundColorItem->setBackground(QBrush(Qt::red));
		foregroundColorItem->setBackground(QBrush(Qt::cyan));
	}
    ui.editorColorList->resizeColumnsToContents();
    ui.editorColorList->resizeRowsToContents();

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

