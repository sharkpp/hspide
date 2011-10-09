#include <QCryptographicHash>
#include <QtDebug>
#include "solution.h"
#include "project.h"
#include "editor.h"
#include "solutionreader.h"

/////////////////////////////////////////////////////////////////////

CSolution::CSolution(QObject *parent)
	: CProjectItem(parent)
{
}

CSolution::~CSolution()
{
}

// �\�����[�V������ǂݍ���
bool CSolution::load(const QString & filename)
{
	CSolutionReader reader;
	return reader.load(filename, *this);
}

// �\�����[�V������ۑ�
bool CSolution::save(const QString & filename)
{
	return false;
}

// �\�����[�V�����Ƀv���W�F�N�g��ǉ�
CProject * CSolution::append(const QString & filename)
{
	CProject * newProject = new CProject(this);

	newProject->setPath(filename);

	if( !filename.isEmpty() ) {
		newProject->load(filename);
	}

	appendRow(newProject);

	return newProject;
}

// �\�����[�V��������v���W�F�N�g�����O
bool CSolution::remove(const QString & filename)
{
//	QSharedPointer<CProject> oldProject = mProjects[filename];
//	mProjects.remove(filename);
	return false;
}

// �\�����[�V�����փG�f�B�^���֘A�t��
bool CSolution::openFile(CEditor* editor)
{
	for(int row = 0, rowNum = rowCount();
		row < rowNum; row++)
	{
		if( CProject * project = dynamic_cast<CProject*>(child(row)) )
		{
			project->openFile(editor);
		}
	}
	return true;
}

// �\�����[�V��������G�f�B�^������
bool CSolution::closeFile(CEditor* editor)
{
	for(int row = 0, rowNum = rowCount();
		row < rowNum; row++)
	{
		if( CProject * project = dynamic_cast<CProject*>(child(row)) )
		{
			project->closeFile(editor);
		}
	}
	return true;
}

// �\�����[�V���������擾
QString CSolution::title() const
{
	return QString();
}

// �\�����[�V���������擾
QString CSolution::filename() const
{
	return QString();
}

// �v���W�F�N�g�����擾
int CSolution::count() const
{
	return rowCount();
}

// �v���W�F�N�g���擾
CProject & CSolution::at(int index) const
{
	return *dynamic_cast<CProject*>(child(index));
}

