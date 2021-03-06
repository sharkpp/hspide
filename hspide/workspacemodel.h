#include <QObject>
#include <QAbstractItemModel>
#include "workspaceitem.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef INCLUDE_GUARD_B2184834_3FFB_4DE2_91B7_4DC346EC0EF1
#define INCLUDE_GUARD_B2184834_3FFB_4DE2_91B7_4DC346EC0EF1

class QMimeData;

class CWorkSpaceModel
	: public QAbstractItemModel
{
	friend class CWorkSpaceItem;

	Q_OBJECT

	CWorkSpaceItem*	rootItem;

public:

	CWorkSpaceModel(QObject* parent = NULL);

 // 読み込み・保存

	bool load(const QString& fileName);
	bool save(const QString& fileName);

	// ソリューションが存在するか
	bool isSolutionExist() const;

	// ソリューションを追加
	CWorkSpaceItem* insertSolution();

	// プロジェクトの追加
	CWorkSpaceItem* appendProject(const QString& fileName = QString());

	// プロジェクトの削除

	// ファイルの追加
	CWorkSpaceItem* appendFile(const QString& fileName, CWorkSpaceItem* parentItem);

	// ファイルの削除

	// 削除
	bool remove(CWorkSpaceItem* item);

	// ソリューションを取得
	CWorkSpaceItem* solution();

  // QAbstractItemModel オーバーライド

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QMap<int, QVariant> itemData(const QModelIndex&index) const;
	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex& index ) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	virtual Qt::ItemFlags flags(const QModelIndex& index) const;
	virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);
	virtual Qt::DropActions supportedDropActions() const;

	virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
	virtual bool removeRows(int row, int count, const QModelIndex& parent);

	bool insertRow(int row, CWorkSpaceItem* item, const QModelIndex& parent = QModelIndex());
	bool insertRows(int row, const QVector<CWorkSpaceItem*>& items, const QModelIndex& parent = QModelIndex());

	bool appendRow(CWorkSpaceItem* item, const QModelIndex& parent = QModelIndex());
	bool appendRows(const QVector<CWorkSpaceItem*>& items, const QModelIndex& parent = QModelIndex());

private:

	CWorkSpaceItem* getItem(const QModelIndex& index) const;

public slots:

	void onLoadComplete();
	void onSaveComplete();

signals:

	void loadComplete(CWorkSpaceItem* item);
	void saveComplete(CWorkSpaceItem* item);
};

#endif // !defined(INCLUDE_GUARD_B2184834_3FFB_4DE2_91B7_4DC346EC0EF1)
