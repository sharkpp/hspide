#include <QObject>
#include <QString>
#include <QMap>
#include <QSharedPointer>

#ifndef INCLUDE_GUARD_3CBB7292_EDC6_4258_B45F_54C680152079
#define INCLUDE_GUARD_3CBB7292_EDC6_4258_B45F_54C680152079

class CProject;
class CEditor;

class CSolution
	: public QObject
{
	Q_OBJECT

public:

	typedef struct {
	} Config;

private:

	Config	mConfig;

	QMap<QString, QSharedPointer<CProject> >	mProjects;

public:

	CSolution(QObject *parent, const CSolution::Config & config);

	~CSolution();

	// �\�����[�V������ǂݍ���
	bool load();

	// �\�����[�V������ۑ�
	bool save(const QString & filename);

	// �\�����[�V�����Ƀv���W�F�N�g��ǉ�
	bool append(const QString & filename = QString());

	// �\�����[�V��������v���W�F�N�g�����O
	bool remove(const QString & filename);

	// �\�����[�V�����փG�f�B�^���֘A�t��
	bool openFile(CEditor* editor);

	// �\�����[�V��������G�f�B�^������
	bool closeFile(CEditor* editor);

	// �\�����[�V���������擾
	QString title();

	// �\�����[�V���������擾
	QString filename();

	// �v���W�F�N�g�����擾
	int count();

	// �v���W�F�N�g���擾
	const CProject & at(int index) const;

	// �\�����[�V�����̃r���h
	void build();

	// �\�����[�V�����̎��s
	void run();

public slots:

	// �v���W�F�N�g�̃r���h�J�n
	void buildStartProject();

	// �v���W�F�N�g�̃r���h����
	void buildFinishedProject(bool successed);

signals:

	void buildStart();
	void buildFinished(bool successed);

};

#endif // defined(INCLUDE_GUARD_3CBB7292_EDC6_4258_B45F_54C680152079)
