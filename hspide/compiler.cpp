#include <QDebug>
#include <QDir>
#include "compiler.h"
#include "workspaceitem.h"

CCompiler::CCompiler(QObject *parent)
	: QObject(parent)
	, mHspCompPath(QDir::toNativeSeparators("./"))
	, mHspPath(QDir::toNativeSeparators("./"))
	, mHspCommonPath(QDir::toNativeSeparators("./common/"))
	, mProcess(NULL)
	, mProcessForListingSymbols(NULL)
{
	updateCompilerPath();
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
	updateCompilerPath();
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

void CCompiler::updateCompilerPath()
{
	QString program = mHspCompPath + "hspcmp";
	QStringList arguments;
	arguments << "--symbol-put";

	delete mProcessForListingSymbols;
	mProcessForListingSymbols = new QProcess(this);

	mProcessForListingSymbols->start(program, arguments);

	if( !mProcessForListingSymbols->waitForStarted() ) {
		return;
	}

	// �����������̒ʒm��o�^
	connect(mProcessForListingSymbols, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(listedSymbolsFinished(int,QProcess::ExitStatus)));
}

// �V���{���ꗗ���擾
const QVector<QStringList> & CCompiler::symbols() const
{
	return mSymbols;
}

// �v���W�F�N�g���r���h
void CCompiler::build(CWorkSpaceItem * project)
{
	QString filename;
	//if( !project->getMainSource(filename) )
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

// �V���{���̎擾����
void CCompiler::listedSymbolsFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	QString tmp(mProcessForListingSymbols->readAllStandardOutput());

	mSymbols.clear();

	for(QStringListIterator ite(tmp.split("\n"));
		ite.hasNext(); )
	{
		QString line = ite.next();
		
		if( !line.contains(",") ) {
			continue;
		}

		// �R���p�C���̃��b�Z�[�W�o�b�t�@��HSP�Ŏg�p�����V���{�������o�͂��܂��B
		// �o�͂́A�u�V���{����,sys[|var/func/macro][|1/2]�v�̌`���ɂȂ�܂��B

		QStringList keywords = line.trimmed().split(QRegExp(",|\\|"));

		for(QMutableStringListIterator ite2(keywords);
			ite2.hasNext(); )
		{
			QString & keyword = ite2.next();
			keyword = keyword.trimmed();
		}

		mSymbols.push_back(keywords);
	}

//	qDebug() << mSymbols;

	delete mProcessForListingSymbols;
}

// �v���W�F�N�g�̃r���h����
void CCompiler::buildFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	// �V�O�i������
	emit buildFinished(QProcess::NormalExit == exitStatus);
	delete mProcess;
}

// �r���h���̏o�͂��擾
void CCompiler::buildReadOutput()
{
	QString tmp(mProcess->readAllStandardOutput());
	emit buildOutput(tmp);
}
