#include <QTextEdit>

class COutputDock
	: public QWidget
{
	Q_OBJECT

	QTextEdit * mEditor;

public:

	COutputDock(QWidget *parent = 0);

	// o—Í‚É•¶š—ñ‚ğ’Ç‹L
	void output(const QString & text);
	void outputCrLf(const QString & text);

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

private:

};
