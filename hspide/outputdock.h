#include <QTextEdit>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class COutputDock
	: public QWidget
{
	Q_OBJECT

	QTextEdit * editorWidget;

public:

	COutputDock(QWidget *parent = 0);

	// 出力をクリア
	void clear();

	// 出力に文字列を追記
	void output(const QString & text);
	void outputCrLf(const QString & text);

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

private:

};
