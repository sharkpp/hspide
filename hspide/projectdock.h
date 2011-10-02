#include <QTreeView>

class CSolution;

class CProjectDock
	: public QWidget
{
	Q_OBJECT

	QTreeView* mTree;

public:

	CProjectDock(QWidget *parent = 0);

	// ƒ\ƒŠƒ…[ƒVƒ‡ƒ“‚ğŒ‹‡
	bool setSolution(CSolution * solution);

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

	void doubleClickedTree(const QModelIndex & inde);

signals:

	void oepnProjectFileItem(const QString & filename);

private:

};
