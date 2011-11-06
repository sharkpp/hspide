#include <QTreeView>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CWorkSpaceModel;
class CWorkSpaceItem;

class CProjectDock
	: public QWidget
{
	Q_OBJECT

	QTreeView* mTree;

public:

	CProjectDock(QWidget *parent = 0);

	// ソリューションを結合
	bool setWorkSpace(CWorkSpaceModel * workspace);

	// アイテムを選択
	void selectItem(CWorkSpaceItem * item);

	// 現在のファイルを取得
	CWorkSpaceItem * currentFile();

	// 現在のプロジェクトを取得
	CWorkSpaceItem * currentProject();


protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

	void doubleClickedTree(const QModelIndex & inde);

signals:

	void oepnItem(CWorkSpaceItem * item);

private:

};
