#include "newfiledialog.h"
#include <QStandardItemModel>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QString>

CNameOnlyFileSystemModel::CNameOnlyFileSystemModel(QObject * parent)
	: QFileSystemModel(parent)
{
}

QVariant CNameOnlyFileSystemModel::data(const QModelIndex & index, int role) const
{
	if( Qt::DisplayRole == role &&
		0 == index.column()) // ファイル名カラム
	{
		// ファイル名のみ返す
		return fileInfo(index).baseName();
	}

	return QFileSystemModel::data(index, role);
}


CNewFileDialog::CNewFileDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setupTemplateList();
}

void CNewFileDialog::setupTemplateList()
{
	//QStandardItemModel * model = new QStandardItemModel();
	//QFileSystemModel modelFs;

	//QFileInfo appPath(qApp->applicationDirPath());
	//modelFs.setRootPath(qApp->applicationDirPath());
	//QModelIndex parentIndex = modelFs.index(qApp->applicationDirPath());

	//for(int i = 0; i < 10/*modelFs.rowCount(parentIndex)*/; ++i)
	//{
	//	QModelIndex index = modelFs.index(i, 0, parentIndex);
	//	QStandardItem * item = new QStandardItem(modelFs.fileIcon(index), modelFs.fileName(index));
	//	item->setData(modelFs.fileName(index));
	//	model->appendRow(item);
	//}

	QString path = qApp->applicationDirPath() + "/templates";
	QFileSystemModel * model = new CNameOnlyFileSystemModel(this);
	QModelIndex parentIndex = model->setRootPath(path);

	ui.templateList->setMovement(QListView::Static);
	ui.templateList->setModel(model);
	ui.templateList->setRootIndex(parentIndex);
}

void CNewFileDialog::onSelectFileName()
{
	QString fileName
		= QFileDialog::getSaveFileName(this,
			tr("Open File"), "", tr("Hot Soup Processor Files (*.hsp *.as)"));

	if( !fileName.isEmpty() )
	{
		ui.fileName->setText(QDir::toNativeSeparators(fileName));
	}
}

void CNewFileDialog::onChangeTemplate(const QModelIndex & index)
{
	QFileSystemModel* model = dynamic_cast<QFileSystemModel*>(ui.templateList->model());
	QFileInfo path(ui.fileName->text());
	QString dirName = path.dir().path();
	QString newPath
		= QDir::toNativeSeparators(
			(dirName.isEmpty() || "." == dirName ? "" : dirName + "/") +
			path.baseName() + "." +
			model->fileInfo(index).suffix());
	// 拡張子のみ変更
	ui.fileName->setText(newPath);
}