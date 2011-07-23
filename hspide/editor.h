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

	// �t�@�C������ǂݍ���
	bool load(const QString & filepath);

	// �t�@�C���ɕۑ�
	bool save(const QString & filepath = QString());

//	// �ꎞ�t�@�C���ɕۑ�
//	bool saveTemporary();
//
//	// �ꎞ�t�@�C�����폜
//	bool clearTemporary();

	// �t�@�C���p�X���擾
	const QString & filePath() const;

	// �t�@�C�������擾
	QString fileName() const;

protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

private:

};
