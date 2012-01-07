#include "varinfodock.h"
#include <QResizeEvent>
#include <QtGui>

CVariableInfoDock::CVariableInfoDock(QWidget *parent)
	: QWidget(parent)
	, m_keyBase(1)
{
	QStandardItemModel* model;
	listWidget = new QTreeView(this);
//	listWidget->setRootIsDecorated(false);
	listWidget->setModel(model = new QStandardItemModel());
	listWidget->setEditTriggers(QTreeView::NoEditTriggers);
	model->invisibleRootItem()->setColumnCount(ColumnCount);
	model->setHeaderData(NameColumn,        Qt::Horizontal, tr("Name"));
	model->setHeaderData(TypeColumn,        Qt::Horizontal, tr("Type"));
	model->setHeaderData(DescriptionColumn, Qt::Horizontal, tr("Description"));
	connect(listWidget, SIGNAL(expanded(const QModelIndex &)), this, SLOT(onTreeExpanded(const QModelIndex &)));
	setEnable(false);
}

void CVariableInfoDock::resizeEvent(QResizeEvent * event)
{
	listWidget->resize(event->size());
}

void CVariableInfoDock::setEnable(bool enable)
{
	listWidget->setEnabled(enable);
}

void CVariableInfoDock::clear()
{
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(listWidget->model());
	QStandardItem* root = model->invisibleRootItem();
	while( root->rowCount() ) {
		root->removeRow(0);
	}
}

void CVariableInfoDock::update(const VARIABLE_INFO_TYPE & varInfo)
{
	QString path = toPath(varInfo.name);
	QStandardItem* item = getItem(path);
	setItem(item, TypeColumn,        varInfo.typeName);
	setItem(item, DescriptionColumn, varInfo.description);
	if( varInfo.lengthOf != item->rowCount() ) {
	//	while( item->rowCount() ) {
	//		item->removeRow(0);
	//	}
		// 子要素を追加
		for(int i = 0; i < varInfo.lengthOf; i++) {
			QString newPath = QString("%1/%2").arg(path).arg(i);
			QStandardItem* item_ = getItem(newPath);
		}
	}
	m_varInfo[item->data().toLongLong()] = varInfo;
}

// 指定したパスのアイテムを取得
QStandardItem* CVariableInfoDock::getItem(const QString & valueName)
{
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(listWidget->model());
	QStandardItem* parent = model->invisibleRootItem();
	QStandardItem* item;
	QStringList key = valueName.split("/");
	QList<QStandardItem*> itemPath;

	itemPath.push_back(parent);

	for(int row = 0; row < parent->rowCount(); row++)
	{
		item = parent->child(row);
		if( key.front() == item->text() )
		{
			itemPath.push_back(item);
			key.pop_front();
			// 全てたどれた？
			if( key.empty() ) {
				return item;
			}
			// 一つもぐる
			parent = item;
			row    = -1;
			continue;
		}
	}

	for(; !key.empty() ;)
	{
		item   = new QStandardItem(key.front());
		item->setData(m_keyBase++);
		parent = itemPath.back();
		// アイテム追加
		parent->setColumnCount(ColumnCount);
		parent->appendRow(item);
		// 下のレベルに移動
		itemPath.push_back(item);
		key.pop_front();
	}

	return item;
}

// アイテムの指定のカラムに値をセット
void CVariableInfoDock::setItem(QStandardItem* item, ColumnType type, const QString & value)
{
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(listWidget->model());
	model->setData(model->index(item->index().row(), int(type), item->index().parent()), value);
}

// 変数名からパスに変換
QString CVariableInfoDock::toPath(const QString& varName)
{
	QRegExp re("([^@]+)(@(.*))?\\((.+),(.+),(.+),(.+)\\)");
	QString path;

	if( 0 <= re.indexIn(varName) )
	{
		int index[4];
		QString varName = re.cap(1); // 変数名
		QString modName = re.cap(3); // モジュール名
		index[0]        = re.cap(4).toInt(); // 配列1
		index[1]        = re.cap(5).toInt(); // 配列2
		index[2]        = re.cap(6).toInt(); // 配列3
		index[3]        = re.cap(7).toInt(); // 配列4
		path = QString("%1/%2").arg(modName.isEmpty()?"@":modName).arg(varName);
		for(int i = 0; i < 4 && 0 <= index[i]; i++) {
			path += QString("/%1").arg(index[i]);
		}
	}
	return path;
}

void CVariableInfoDock::onTreeExpanded(const QModelIndex& index)
{
	QStandardItem* item = static_cast<QStandardItem*>(index.internalPointer());
	               item = item->child(index.row());
qlonglong x = item->data().toLongLong();
qDebug() << x << m_varInfo;
	QString varName = m_varInfo[item->data().toLongLong()].name;

	QRegExp re("([^@]+)(@(.*))?\\((.+),(.+),(.+),(.+)\\)");

	if( 0 <= re.indexIn(varName) )
	{
		int index[4];
		QString varName = re.cap(1); // 変数名
		QString modName = re.cap(3); // モジュール名
		index[0]        = re.cap(4).toInt(); // 配列1
		index[1]        = re.cap(5).toInt(); // 配列2
		index[2]        = re.cap(6).toInt(); // 配列3
		index[3]        = re.cap(7).toInt(); // 配列4

		varName = modName.isEmpty() ? "" : "@" + modName;

		emit requestVariableInfo(varName, index);
	}
}
