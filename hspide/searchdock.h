#include <QTextEdit>

class CSearchDock
	: public QWidget
{
	Q_OBJECT

	QTextEdit * mEditor;

public:

	CSearchDock(QWidget *parent = 0);

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

private:

};
