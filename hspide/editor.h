#include <QTextEdit>
#include <QString>
#include <QResizeEvent>

class CEditor
	: public QWidget
{
	Q_OBJECT

	QTextEdit* mEditor;

	QString	mFilePath;

public:

	CEditor(QWidget *parent = 0);

	// ファイルから読み込み
	bool load(const QString & filepath);

	// ファイルに保存
	bool save(const QString & filepath = QString());

//	// 一時ファイルに保存
//	bool saveTemporary();
//
//	// 一時ファイルを削除
//	bool clearTemporary();

	// ファイルパスを取得
	const QString & filePath() const;

	// ファイル名を取得
	QString fileName() const;

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

private:

};
