#include <QTreeView>

class CSolution;

class CProjectDock
	: public QWidget
{
	Q_OBJECT

	QTreeView* mTree;

public:

	CProjectDock(QWidget *parent = 0);

	// ソリューションを結合
	bool setSolution(CSolution * solution);

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

private:

};
