#include <QTextEdit>

class COutputDock
	: public QWidget
{
	Q_OBJECT

	QTextEdit * mEditor;

public:

	COutputDock(QWidget *parent = 0);

	// �o�͂ɕ������ǋL
	void output(const QString & text);
	void outputCrLf(const QString & text);

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

private:

};
