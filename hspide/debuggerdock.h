#include <QTextEdit>

class CDebuggerDock
	: public QWidget
{
	Q_OBJECT

	QTextEdit * mEditor;

public:

	CDebuggerDock(QWidget *parent = 0);

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

private:

};
