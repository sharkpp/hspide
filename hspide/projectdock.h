#include <QTextEdit>

class CProjectDock
	: public QWidget
{
	Q_OBJECT

	QTextEdit * mEditor;

public:

	CProjectDock(QWidget *parent = 0);

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

private:

};
