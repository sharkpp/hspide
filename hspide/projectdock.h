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

	// ƒ\ƒŠƒ…[ƒVƒ‡ƒ“‚ðŒ‹‡
	bool setWorkSpace(CWorkSpaceModel * workspace);

	void selectItem(CWorkSpaceItem * item);

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

	void doubleClickedTree(const QModelIndex & inde);

signals:

	void oepnItem(CWorkSpaceItem * item);

private:

};
