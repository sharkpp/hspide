
#include <QtGui>

class EditorWidget : public QWidget
{
	Q_OBJECT

	QTextEdit * textEditor;

public:

	EditorWidget(QWidget *parent = 0);

	void setPlainText(const QString &text);

protected:

public slots:

private:

};
