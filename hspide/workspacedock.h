#include <QTreeView>
#include <QList>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CWorkSpaceModel;
class CWorkSpaceItem;

class CWorkSpaceDock
	: public QWidget
{
	Q_OBJECT

	QTreeView* treeWidget;

public:

	CWorkSpaceDock(QWidget* parent = 0);

	// ソリューションを結合
	bool setWorkSpace(CWorkSpaceModel* workspace);

	// アイテムを選択
	void selectItem(CWorkSpaceItem* item);

	// 現在のアイテムを取得
	CWorkSpaceItem* currentItem();

	// 現在のソリューションを取得
	CWorkSpaceItem* currentSolution();

	// 現在のプロジェクトを取得
	CWorkSpaceItem* currentProject();

	// プロジェクトを取得
	QList<CWorkSpaceItem*> projects();

	// 現在のファイルを取得
	CWorkSpaceItem* currentFile();

	QTreeView* tree();

protected:

	virtual void resizeEvent(QResizeEvent* event);

protected slots:

	void doubleClickedTree(const QModelIndex& inde);
	void expandedTree(const QModelIndex& index);

signals:

	void oepnItem(CWorkSpaceItem* item);

private:

};

inline
QTreeView* CWorkSpaceDock::tree()
{
	return treeWidget;
}
