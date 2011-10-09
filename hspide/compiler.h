#include <QObject>
#include <QProcess>

#ifndef INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29
#define INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29

class CProject;

class CCompiler
	: public QObject
{
	Q_OBJECT

	QProcess *mProcess;

public:

	CCompiler(QObject *parent = 0);

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
