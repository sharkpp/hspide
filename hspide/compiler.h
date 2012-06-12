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

typedef QMap<QUuid, QSet<int> > CBreakPointInfo;
typedef QMap<QString, QUuid> CUuidLookupInfo;

class CCompiler
	: public QObject
{
	Q_OBJECT

	QString m_hspCompPath;		// HSP�R���p�C��DLL�ւ̃p�X
	QString m_hspPath;			// HSP�C���X�g�[���f�B���N�g���ւ̃p�X
	QString m_hspCommonPath;	// HSP Common�t�H���_�ւ̃p�X

	QString m_buildAfterRunArgs;		// �r���h��Ɏ��s���s�����H

	QVector<QStringList> m_highlightSymbols;	// �擾�����V���{���̈ꗗ

	QMap<quint64, CWorkSpaceItem*> m_targetsTemp;
	QList<QProcess*> m_compilerProcesses;

	QMap<QProcess*, int> m_buildOrder;

	QLocalServer* m_server;

public:

	CCompiler(QObject *parent = 0);

	// �V���{���ꗗ���擾
	const QVector<QStringList>& symbols() const;

	// �r���h
	bool build(CWorkSpaceItem* targetItem, bool debugMode);

	// ���s
	bool run(CWorkSpaceItem* targetItem, bool debugMode);

	// �P��t�@�C�����R���p�C��
	void compile();

	CWorkSpaceItem* getTargetItem(qint64 id);

protected:

	void updateCompilerPath();

	bool execCompiler(CWorkSpaceItem * targetItem, bool buildAfterRun, bool debugMode);

	// �r���h�����擾
	int buildOrder(QProcess* process) const;

public slots:

	void updateConfiguration(const Configuration& info);

	// �V���{���̎擾����
	void listedSymbolsFinished(int exitCode, QProcess::ExitStatus exitStatus);

	void buildError(QProcess::ProcessError);
	void buildFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void buildReadOutput();

	void attachDebugger();

signals:

	void buildStart(int buildOrder, const QString & filePath);
	void buildFinished(int buildOrder, bool status);
	void buildOutput(int buildOrder, const QString & output);

	void attachedDebugger(CDebugger* debugger);

	void updatedSymbols();

};

#endif // !defined(INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29)
