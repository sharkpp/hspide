#include "solution.h"
#include "project.h"

CSolution::CSolution(const CSolution::Config & config)
	: mConfig(config)
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
	connect(&*newProject, SIGNAL(buildStart()),        this, SLOT(buildStartProject()));
	connect(&*newProject, SIGNAL(buildFinished(bool)), this, SLOT(buildFinishedProject(bool)));

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

// �\�����[�V�����̎��s
void CSolution::run()
{
}
