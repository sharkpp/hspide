#include <QObject>
#include <QProcess>
#include <QString>

#ifndef INCLUDE_GUARD_DE12DB44_EA81_45D1_9E18_9CCA26479DE7
#define INCLUDE_GUARD_DE12DB44_EA81_45D1_9E18_9CCA26479DE7

class CProject
	: public QObject
{
	Q_OBJECT

public:

	typedef struct {
		QString	compilerPath;
	} Config;

private:

	Config	mConfig;

public:

	CProject(const CProject::Config & config);

	~CProject();

	// �v���W�F�N�g��ǂݍ���
	bool load();

	// �v���W�F�N�g��ۑ�
	bool save(const QString & filename = QString());

	// �v���W�F�N�g�Ƀt�@�C����ǉ�
	bool append(const QString & filename);

	// �v���W�F�N�g���珜�O
	bool remove(const QString & filename);

	// �v���W�F�N�g���̃t�@�C�����������ɓǂݍ���
	bool openFile(const QString & filename);

	// �v���W�F�N�g���̃t�@�C����ۑ�
	bool saveFile(const QString & filename);

	// �v���W�F�N�g���̃t�@�C�������
	bool closeFile(const QString & filename);

	// �v���W�F�N�g���r���h
	void build();

public slots:

	// �v���W�F�N�g�̃r���h����
	void buildFinished(int exitCode, QProcess::ExitStatus exitStatus);

signals:

	void buildStart();
	void buildFinished(bool status);

private:

	// �v���W�F�N�g���̃t�@�C�����ꎞ�I�ɕۑ�
	bool saveTemp();

	// �v���W�F�N�g���̈ꎟ�ۑ��t�@�C�����N���A
	bool clearTemp();

};

#endif // defined(INCLUDE_GUARD_DE12DB44_EA81_45D1_9E18_9CCA26479DE7)
