#include <QDebug>
#include <QDir>
#include <QLocalSocket>
#include "compiler.h"
#include "workspaceitem.h"
#include "documentpane.h"
#include "debugger.h"

CCompiler::CCompiler(QObject *parent)
	: QObject(parent)
	, m_uuid(QUuid::createUuid())
	, m_targetItem(NULL)
	, m_compiler(NULL)
	, m_hspCompPath(QDir::toNativeSeparators("./"))
	, m_hspPath(QDir::toNativeSeparators("./"))
	, m_hspCommonPath(QDir::toNativeSeparators("./common/"))
{
	updateCompilerPath();

	// �ݒ�̕ύX�ʎj�̓o�^�Ɛݒ肩��̏���������
	connect(&theConf, SIGNAL(updateConfiguration(const Configuration&)),
	        this,  SLOT(updateConfiguration(const Configuration&)));
	updateConfiguration(theConf);
}

const QUuid& CCompiler::uuid() const
{
	return m_uuid;
}

// �V���{���ꗗ�̎擾
bool CCompiler::collectSymbols()
{
	QString program = m_hspCompPath + "hspcmp.exe";

	if( !QFileInfo(program).exists() )
	{
		return false;
	}

	QStringList arguments;
	arguments << "--symbol-put";

	QProcess* process = new QProcess(this);

	// �����������̒ʒm��o�^
	connect(process, SIGNAL(finished(int,QProcess::ExitStatus)),
			this, SLOT(listedSymbolsFinished(int,QProcess::ExitStatus)));

	process->start(program, arguments);

	if( !process->waitForStarted() )
	{
		delete process;
		return false;
	}
	return true;
}

// �V���{���ꗗ���擾
const CSymbolsList& CCompiler::symbols() const
{
	return m_highlightSymbols;
}

CWorkSpaceItem* CCompiler::compileItem()
{
	return m_targetItem;
}

// �R���p�C���Ăяo��
bool CCompiler::compile(CWorkSpaceItem * targetItem, bool debugMode)
{
	QString workDir = QDir::currentPath(); // ����Ȃ�����

	QString filename = targetItem->path();

	m_targetItem = targetItem;

	// �V�O�i������
	emit compileStarted(filename);

	switch( targetItem->type() )
	{
	case CWorkSpaceItem::Project:
		break;
	default:
		emit compileFailure();
		return false;
	}

	bool tempSave = false;

	// ����̏ꍇ�͈ꎞ�I�ȃt�@�C���ɕۑ�
	if( targetItem->isUntitled() )
	{
		CDocumentPane* document = targetItem->assignDocument();

		if( !document )
		{
			emit compileFailure();
			return false;
		}

		// �ꎞ�I�ȃt�@�C���ɕۑ�
		tempSave = true;
		filename = QDir::toNativeSeparators(workDir + "/hsptmp"); //���j�[�N�Ȗ��O�ɕς���
		document->save(filename);
	}

	QProcess* hspcmp = new QProcess(this);

	QString program = m_hspCompPath + "hspcmp";
	QStringList arguments;
	arguments << "-C" << m_hspCommonPath
	          << "-H" << m_hspPath;
	if( debugMode ) { // �f�o�b�O���[�h
		arguments << "-d";
	}
	if( tempSave ) { // �ꎞ�I�ȃt�@�C���ɕۑ�
		arguments << "-o" << "obj"
		          << "-r" << QString("?%1").arg(targetItem->uuid().toString());
	} else {
		arguments << "-r" << QString("?%1").arg(targetItem->uuid().toString());
	}
	arguments << filename;

	// �����������̒ʒm��o�^
	connect(hspcmp, SIGNAL(error(QProcess::ProcessError)),      this, SLOT(hspcmpError(QProcess::ProcessError)));
	connect(hspcmp, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(hspcmpFinished(int,QProcess::ExitStatus)));
	connect(hspcmp, SIGNAL(readyReadStandardOutput()),          this, SLOT(hspcmpReadOutput()));

	m_compiler   = hspcmp;

	hspcmp->setWorkingDirectory(workDir);
	hspcmp->start(program, arguments);

	if( !hspcmp->waitForStarted() )
	{
		delete hspcmp;
		emit compileFailure();
		m_compiler   = NULL;
		return false;
	}

	return true;
}

void CCompiler::updateCompilerPath()
{
}

void CCompiler::updateConfiguration(const Configuration& info)
{
	bool update = false;
	QString hspPath       = QDir::toNativeSeparators(info.hspPath());
	QString hspCompPath   = QDir::toNativeSeparators(info.compilerPath());
	QString hspCommonPath = QDir::toNativeSeparators(info.hspCommonPath());
	update |= m_hspPath       != hspPath;
	update |= m_hspCompPath   != hspCompPath;
	update |= m_hspCommonPath != hspCommonPath;
	m_hspCompPath   = hspPath;
	m_hspPath       = hspCompPath;
	m_hspCommonPath = hspCommonPath;
	if( update ) {
		updateCompilerPath();
	}
}

void CCompiler::hspcmpError(QProcess::ProcessError)
{
}

void CCompiler::hspcmpFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	QProcess* hspcmp = qobject_cast<QProcess*>(sender());

	// �V�O�i������
	if( QProcess::NormalExit == exitStatus )
	{
		emit compileSuccessful();
	}
	else
	{
		emit compileFailure();
	}

	delete hspcmp;
}

void CCompiler::hspcmpReadOutput()
{
	QProcess* hspcmp = qobject_cast<QProcess*>(sender());
	QString   tmp(hspcmp->readAllStandardOutput());
	emit compileOutput(tmp);
}

// �V���{���̎擾����
void CCompiler::listedSymbolsFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	QProcess* hspcmp = qobject_cast<QProcess*>(sender());
	QString tmp(hspcmp->readAllStandardOutput());

	m_highlightSymbols.clear();

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

		m_highlightSymbols.push_back(keywords);
	}

//	qDebug() << mSymbols;

	emit updatedSymbols();

	hspcmp->deleteLater();
}

