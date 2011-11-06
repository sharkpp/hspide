#include <QObject>
#include <QAbstractItemModel>
#include "workspaceitem.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CWorkSpaceModel
	: public QAbstractItemModel
{
	friend class CWorkSpaceItem;

	Q_OBJECT

	CWorkSpaceItem*	rootItem;

public:

	CWorkSpaceModel(QObject * parent = NULL);

 // 読み込み・保存

	bool load(const QString & fileName);
	bool save(const QString & fileName);

	// プロジェクトの追加
	CWorkSpaceItem * appendProject(const QString & fileName = QString());

	// プロジェクトの削除

	// ファイルの追加
	CWorkSpaceItem * appendFile(const QString & fileName, CWorkSpaceItem * parentItem);

	// ファイルの削除

  // QAbstractItemModel オーバーライド

	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex & index ) const;
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

	virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

private:

	CWorkSpaceItem * getItem(const QModelIndex & index) const;

};
