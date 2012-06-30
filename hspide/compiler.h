#include <QObject>
#include <QString>
#include <QProcess>
#include <QVector>
#include <QMap>
#include <QLocalServer>
#include <QUuid>
#include "global.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29
#define INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29

class CWorkSpaceItem;
class CDebugger;
class QTemporaryFile;

typedef QVector<QStringList> CSymbolsList;

class CCompiler
	: public QObject
{
	Q_OBJECT

	QUuid			m_uuid;

	QString			m_hspCompPath;		// HSP�R���p�C��DLL�ւ̃p�X
	QString			m_hspPath;			// HSP�C���X�g�[���f�B���N�g���ւ̃p�X
	QString			m_hspCommonPath;	// HSP Common�t�H���_�ւ̃p�X

	CSymbolsList	m_highlightSymbols;	// �擾�����V���{���̈ꗗ

	CWorkSpaceItem*	m_targetItem;
	QProcess*		m_compiler;

	QTemporaryFile*	m_objTemp;

	QStringList		m_hspErrorText;

public:

	CCompiler(QObject *parent = 0);
	virtual ~CCompiler(){qDebug()<<__FUNCTION__<<this;}

	const QUuid& uuid() const;

	QTemporaryFile*	objTemp();

	// �V���{���ꗗ�̎擾
	bool collectSymbols();

	// �V���{���ꗗ���擾
	const CSymbolsList& symbols() const;

	// �G���[��������擾
	const QStringList& hspErrorTexts() const;


	// �R���p�C��
	bool compile(CWorkSpaceItem* targetItem, bool debugMode);

	// �R���p�C��
	bool compileCancel();

	// �R���p�C���Ώۂ̃A�C�e�����擾
	CWorkSpaceItem* compileItem();

protected:

	void updateCompilerPath();

	bool execCompiler(CWorkSpaceItem* targetItem, bool buildAfterRun, bool debugMode);

	// �r���h�����擾
	int buildOrder(QProcess* process) const;

public slots:

	void updateConfiguration(const Configuration& info);

	// �V���{���̎擾����
	void listedSymbolsFinished(int exitCode, QProcess::ExitStatus exitStatus);

	void hspcmpError(QProcess::ProcessError);
	void hspcmpFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void hspcmpReadOutput();

signals:

	void compileStarted(const QUuid& uuid);
	void compileSuccessful();
	void compileFailure();
	void compileOutput(const QString& output);

	void updatedSymbols();
};

#endif // !defined(INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29)
