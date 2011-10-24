#include <QObject>
#include <QProcess>

#ifndef INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29
#define INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29

class CProject;

class CCompiler
	: public QObject
{
	Q_OBJECT

	QString mHspCompPath;
	QString mHspPath;
	QString mHspCommonPath;

	QProcess *mProcess;

public:

	CCompiler(QObject *parent = 0);

	// �R���p�C���̃p�X���擾
	const QString &  compilerPath() const;
	// �R���p�C���̃p�X���w��
	void setCompilerPath(const QString & path);

	// HSP�f�B���N�g���̃p�X���擾
	const QString &  hspPath() const;
	// HSP�f�B���N�g���̃p�X���w��
	void setHspPath(const QString & path);

	// HSP�f�B���N�g���̃p�X���擾
	const QString &  hspCommonPath() const;
	// HSP common�f�B���N�g���̃p�X���w��
	void setHspCommonPath(const QString & path);

	// �v���W�F�N�g���r���h
	void build(CProject * project);

	// �P��t�@�C�����R���p�C��
	void compile();

protected:

public slots:

	// �v���W�F�N�g�̃r���h����
	void buildFinished(int exitCode, QProcess::ExitStatus exitStatus);

	// �r���h���̏o�͂��擾
	void buildReadOutput();

signals:

	void buildStart();
	void buildFinished(bool status);
	void buildOutput(const QString & output);

};

#endif // !defined(INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29)
