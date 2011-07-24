#include <QTextEdit>

class CSymbolDock
	: public QWidget
{
	Q_OBJECT

	QTextEdit * mEditor;

public:

	CSymbolDock(QWidget *parent = 0);

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

private:

};
