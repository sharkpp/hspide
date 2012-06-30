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

	QString			m_hspCompPath;		// HSPコンパイラDLLへのパス
	QString			m_hspPath;			// HSPインストールディレクトリへのパス
	QString			m_hspCommonPath;	// HSP Commonフォルダへのパス

	CSymbolsList	m_highlightSymbols;	// 取得したシンボルの一覧

	CWorkSpaceItem*	m_targetItem;
	QProcess*		m_compiler;

	QTemporaryFile*	m_objTemp;

	QStringList		m_hspErrorText;

public:

	CCompiler(QObject *parent = 0);
	virtual ~CCompiler(){qDebug()<<__FUNCTION__<<this;}

	const QUuid& uuid() const;

	QTemporaryFile*	objTemp();

	// シンボル一覧の取得
	bool collectSymbols();

	// シンボル一覧を取得
	const CSymbolsList& symbols() const;

	// エラー文字列を取得
	const QStringList& hspErrorTexts() const;


	// コンパイル
	bool compile(CWorkSpaceItem* targetItem, bool debugMode);

	// コンパイル
	bool compileCancel();

	// コンパイル対象のアイテムを取得
	CWorkSpaceItem* compileItem();

protected:

	void updateCompilerPath();

	bool execCompiler(CWorkSpaceItem* targetItem, bool buildAfterRun, bool debugMode);

	// ビルド順を取得
	int buildOrder(QProcess* process) const;

public slots:

	void updateConfiguration(const Configuration& info);

	// シンボルの取得完了
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
