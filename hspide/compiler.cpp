#include <QDir>
#include "compiler.h"
#include "solution.h"
#include "project.h"

CCompiler::CCompiler(QObject *parent)
	: QObject(parent)
	, mProcess(NULL)
{
}

// �v���W�F�N�g���r���h
void CCompiler::build()
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
