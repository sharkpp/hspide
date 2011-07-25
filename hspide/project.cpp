#include "project.h"
#include "editor.h"
#include <QDir>

CProject::CProject(const CProject::Config & config)
	: mConfig(config)
	, mProcess(NULL)
{
}

CProject::~CProject()
{
	delete mProcess;
}

// �v���W�F�N�g��ǂݍ���
bool CProject::load()
{
	return false;
}

// �v���W�F�N�g��ۑ�
bool CProject::save(const QString & filename)
{
	return false;
}

// �v���W�F�N�g�Ƀt�@�C����ǉ�
bool CProject::append(const QString & filename)
{
	return false;
}

// �v���W�F�N�g���珜�O
bool CProject::remove(const QString & filename)
{
	return false;
}

// �v���W�F�N�g�փG�f�B�^���֘A�t��
bool CProject::openFile(CEditor* editor)
{
//	editor->filename()

	return false;
}

// �v���W�F�N�g���̃t�@�C�������
bool CProject::closeFile(CEditor* editor)
{
	return false;
}

// �v���W�F�N�g���r���h
void CProject::build()
{
#ifdef _DEBUG
	QDir::setCurrent(QDir::currentPath() + "\\debug\\");
#endif

	QString program = "./hspcmp";
	QStringList arguments;
	arguments << "-style" << "motif";
	delete mProcess;
	mProcess = new QProcess(this);
	mProcess->start(program, arguments);

	if( !mProcess->waitForStarted() ) {
		emit buildFinished(false);
		delete mProcess;
		return;
	}

	// �����������̒ʒm��o�^
	connect(mProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(buildFinished(int,QProcess::ExitStatus)));
	connect(mProcess, SIGNAL(readyReadStandardOutput()),          this, SLOT(buildReadOutput()));

	// �V�O�i������
	emit buildStart();
}

// �v���W�F�N�g�̃r���h����
void CProject::buildFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	// �V�O�i������
	emit buildFinished(QProcess::NormalExit == exitStatus);
}

// �r���h���̏o�͂��擾
void CProject::buildReadOutput()
{
	QString tmp(mProcess->readAllStandardOutput());
	emit buildOutput(tmp);
}

// �v���W�F�N�g���̃t�@�C�����ꎞ�I�ɕۑ�
bool CProject::saveTemp()
{
	return false;
}

// �v���W�F�N�g���̈ꎟ�ۑ��t�@�C�����N���A
bool CProject::clearTemp()
{
	return false;
}

