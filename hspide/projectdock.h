#include <QTreeWidget>
#include <QAbstractItemModel>

class CProjectDock
	: public QWidget
{
	Q_OBJECT

	QTreeWidget* mTree;

public:

	CProjectDock(QWidget *parent = 0);

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

private:

};
