#include <QDebug>
#include <QDir>
#include "compiler.h"
#include "solution.h"
#include "project.h"

CCompiler::CCompiler(QObject *parent)
	: QObject(parent)
	, mHspCompPath(QDir::toNativeSeparators("./"))
	, mHspPath(QDir::toNativeSeparators("./"))
	, mHspCommonPath(QDir::toNativeSeparators("./common/"))
	, mProcess(NULL)
{
}

// �R���p�C���̃p�X���擾
const QString &  CCompiler::compilerPath() const
{
	return mHspCompPath;
}

// �R���p�C���̃p�X���w��
void CCompiler::setCompilerPath(const QString & path)
{
	mHspCompPath = QDir::toNativeSeparators(path);
}

// HSP�f�B���N�g���̃p�X���擾
const QString &  CCompiler::hspPath() const
{
	return mHspPath;
}

// HSP�f�B���N�g���̃p�X���w��
void CCompiler::setHspPath(const QString & path)
{
	mHspPath = QDir::toNativeSeparators(path);
}

// HSP�f�B���N�g���̃p�X���擾
const QString &  CCompiler::hspCommonPath() const
{
	return mHspCommonPath;
}

// HSP common�f�B���N�g���̃p�X���w��
void CCompiler::setHspCommonPath(const QString & path)
{
	mHspCommonPath = QDir::toNativeSeparators(path);
}

// �v���W�F�N�g���r���h
void CCompiler::build(CProject * project)
{
	QString filename;
	if( !project->getMainSource(filename) )
	{
		return;
	}

	QString program = mHspCompPath + "hspcmp";
	QStringList arguments;
	arguments << "-C" << mHspCommonPath
	          << "-H" << mHspPath
	          << filename;
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
void CCompiler::buildFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	// �V�O�i������
	emit buildFinished(QProcess::NormalExit == exitStatus);
}

// �r���h���̏o�͂��擾
void CCompiler::buildReadOutput()
{
	QString tmp(mProcess->readAllStandardOutput());
	emit buildOutput(tmp);
}
