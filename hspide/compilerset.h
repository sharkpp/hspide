#include <QObject>
#include <QString>
#include <QProcess>
#include <QList>
#include <QLocalServer>
#include <QUuid>
#include "global.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef INCLUDE_GUARD_C2C165C3_7D49_46AF_9C72_CB57C38C5086
#define INCLUDE_GUARD_C2C165C3_7D49_46AF_9C72_CB57C38C5086

class CWorkSpaceItem;
class CDebugger;
class CCompiler;
class QTemporaryFile;

class CCompilerSet
	: public QObject
{
	Q_OBJECT

	typedef struct {
		CWorkSpaceItem*	item;
		bool			debugMode;
		bool			buildAfterRun;
		int				buildOrder;
		QUuid			uuid;
		QString			runtime;
		QString			objName;
		QTemporaryFile*	objTemp;
	} BuildOptionType;

	typedef struct {
		CWorkSpaceItem*	item;
		QUuid			uuid;
		QString			runtime;
		QString			objName;
		QTemporaryFile*	objTemp;
		QProcess*		process;
	} ExecOptionType;

	QString					m_hspPath;
	QString					m_hspCompPath;
	QLocalServer*			m_server;
	QList<CCompiler*>		m_compilers;
	QList<BuildOptionType>	m_buildItems;
	QList<BuildOptionType>	m_buildingItems;
	QList<ExecOptionType>	m_execItems;
	int						m_countOfSuccessful;
	int						m_countOfFailure;

public:

	CCompilerSet(QObject *parent = 0);

	// コンパイラスロット数を設定
	bool setCompilerSlotNum(int num);

	// コンパイラスロット数を取得
	int compilerSlotNum() const;

	// ビルド
	bool build(CWorkSpaceItem* targetItem, bool debugMode);

	// 実行
	bool run(CWorkSpaceItem* targetItem, bool debugMode);

	// ビルド中止
	bool buildCancel();

	// 単一ファイルをコンパイル
	void compile();

	CWorkSpaceItem* getTargetItem(const QString& id) const;

	// エラー文字列を取得
	QString hspErrorText(int errorNo);

protected:

	bool execCompiler(CWorkSpaceItem* targetItem, bool buildAfterRun, bool debugMode);

	bool execBuildDeliverables();

	// ビルド待ちのアイテムからコンパイラに関連付け
	bool assignBuildItem(CCompiler* compiler);

	// コンパイラに関連付けられているビルドオプションを取得
	const BuildOptionType* searchBuildOption(CCompiler* compiler, int* index = NULL) const;

	// コンパイラに関連付けられているビルドオプションを取得
	const ExecOptionType* searchExecOption(QProcess* process, int* index = NULL) const;

public slots:

	void updateConfiguration(const Configuration* conf);

	void compileStarted(const QUuid& uuid);
	void compileSuccessful();
	void compileFailure();
	void compileReadOutput(const QString& output);

	void execError(QProcess::ProcessError);
	void execFinished(int exitCode, QProcess::ExitStatus exitStatus);

	void attachDebugger();

signals:

	void buildStarted();
	void buildStarted(int buildOrder, const QUuid& uuid);
	void buildSuccessful();
	void buildSuccessful(int buildOrder);
	void buildFailure();
	void buildFailure(int buildOrder);
	void buildOutput(int buildOrder, const QString& output);

	void debugStarted();
	void debugFinished();

	void attachedDebugger(CDebugger* debugger);
};

#endif // !defined(INCLUDE_GUARD_C2C165C3_7D49_46AF_9C72_CB57C38C5086)
