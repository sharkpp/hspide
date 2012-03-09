#include <QObject>
#include <QString>
#include <QProcess>
#include <QVector>
#include <QMap>
#include <QLocalServer>
#include <QUuid>
#include "configuration.h"

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

	QString m_hspCompPath;		// HSPコンパイラDLLへのパス
	QString m_hspPath;			// HSPインストールディレクトリへのパス
	QString m_hspCommonPath;	// HSP Commonフォルダへのパス

	QString m_buildAfterRunArgs;		// ビルド後に実行を行うか？

	QVector<QStringList> m_highlightSymbols;	// 取得したシンボルの一覧

	QMap<quint64, CBreakPointInfo> m_breakpoints;
	QMap<quint64, CUuidLookupInfo> m_lookup;
	QList<QProcess*> m_compilerProcesses;

	QLocalServer* m_server;

public:

	CCompiler(QObject *parent = 0);

	// シンボル一覧を取得
	const QVector<QStringList> & symbols() const;

	// 設定更新
	void setConfiguration(const Configuration& info);

	// ビルド
	bool build(CWorkSpaceItem * targetItem, bool debugMode);

	// 実行
	bool run(CWorkSpaceItem * targetItem, bool debugMode);

	// 単一ファイルをコンパイル
	void compile();

	bool getBreakPoint(qint64 id, CBreakPointInfo & bp, CUuidLookupInfo & lookup);

protected:

	void updateCompilerPath();

	bool execCompiler(CWorkSpaceItem * targetItem, bool buildAfterRun, bool debugMode);

public slots:

	void updateConfiguration(const Configuration& info);

	// シンボルの取得完了
	void listedSymbolsFinished(int exitCode, QProcess::ExitStatus exitStatus);

	void buildError(QProcess::ProcessError);
	void buildFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void buildReadOutput();

	void attachDebugger();

signals:

	void buildStart(const QString & filePath);
	void buildFinished(bool status);
	void buildOutput(const QString & output);

	void attachedDebugger(CDebugger* debugger);

	void updatedSymbols();

};

#endif // !defined(INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29)
