#include "solution.h"
#include "project.h"
#include "editor.h"

CSolution::CSolution(QObject *parent, const CSolution::Config & config)
	: QObject(parent)
	, mConfig(config)
{
}

CSolution::~CSolution()
{
}

// �\�����[�V������ǂݍ���
bool CSolution::load()
{
	return false;
}

// �\�����[�V������ۑ�
bool CSolution::save(const QString & filename)
{
	return false;
}

// �\�����[�V�����Ƀv���W�F�N�g��ǉ�
bool CSolution::append(const QString & filename)
{
	CProject::Config config;
//	config.compilerPath = ""

	QSharedPointer<CProject> newProject(new CProject(config));
	mProjects[filename] = newProject;

	// �V�O�i���̒ʒm���o�^
	connect(&*newProject, SIGNAL(buildStart()),                this, SLOT(buildStartProject()));
	connect(&*newProject, SIGNAL(buildFinished(bool)),         this, SLOT(buildFinishedProject(bool)));
	connect(&*newProject, SIGNAL(buildOutput(const QString&)), this, SLOT(buildOutputProject(const QString&)));

	return false;
}

// �\�����[�V��������v���W�F�N�g�����O
bool CSolution::remove(const QString & filename)
{
	QSharedPointer<CProject> oldProject = mProjects[filename];

	// �V�O�i���̒ʒm��̓o�^����
//	disconnect(newProject, );

	mProjects.remove(filename);

	return false;
}

// �\�����[�V�����փG�f�B�^���֘A�t��
bool CSolution::openFile(CEditor* editor)
{
	for(QMap<QString, QSharedPointer<CProject> >::iterator
			ite = mProjects.begin(),
			last= mProjects.end();
		ite != last; ++ite)
	{
		(*ite)->openFile(editor);
	}
	return true;
}

// �\�����[�V��������G�f�B�^������
bool CSolution::closeFile(CEditor* editor)
{
	for(QMap<QString, QSharedPointer<CProject> >::iterator
			ite = mProjects.begin(),
			last= mProjects.end();
		ite != last; ++ite)
	{
		(*ite)->closeFile(editor);
	}
	return true;
}

// �\�����[�V�����̃r���h
void CSolution::build()
{
	for(QMap<QString, QSharedPointer<CProject> >::iterator
			ite = mProjects.begin(),
			last= mProjects.end();
		ite != last; ++ite)
	{
		(*ite)->build();
	}

	emit buildStart();
}

// �v���W�F�N�g�̃r���h�J�n
void CSolution::buildStartProject()
{
}

// �v���W�F�N�g�̃r���h����
void CSolution::buildFinishedProject(bool successed)
{
	emit buildFinished(successed);
}

// �r���h���̏o�͂��擾
void CSolution::buildOutputProject(const QString & output)
{
	emit buildOutput(output);
}

// �\�����[�V�����̎��s
void CSolution::run()
{
}
