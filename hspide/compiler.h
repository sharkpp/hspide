#include <QObject>
#include <QString>
#include <QVector>
#include <QProcess>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29
#define INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29

class CWorkSpaceItem;

class CCompiler
	: public QObject
{
	Q_OBJECT

	QString m_hspCompPath;		// HSPコンパイラDLLへのパス
	QString m_hspPath;			// HSPインストールディレクトリへのパス
	QString m_hspCommonPath;	// HSP Commonフォルダへのパス

	QString m_buildAfterRunArgs;		// ビルド後に実行を行うか？

	QProcess *m_compilerProcess;	// コンパイラプロセス
	QProcess *m_listingSymbolsProcess;	// シンボル取得プロセス

	QVector<QStringList> m_highlightSymbols;	// 取得したシンボルの一覧

public:

	CCompiler(QObject *parent = 0);

	// シンボル一覧を取得
	const QVector<QStringList> & symbols() const;

	// コンパイラのパスを取得
	const QString &  compilerPath() const;
	// コンパイラのパスを指定
	void setCompilerPath(const QString & path);

	// HSPディレクトリのパスを取得
	const QString &  hspPath() const;
	// HSPディレクトリのパスを指定
	void setHspPath(const QString & path);

	// HSPディレクトリのパスを取得
	const QString &  hspCommonPath() const;
	// HSP commonディレクトリのパスを指定
	void setHspCommonPath(const QString & path);

	// ビルド
	bool build(CWorkSpaceItem * targetItem);

	// 実行
	bool run(CWorkSpaceItem * targetItem);

	// 単一ファイルをコンパイル
	void compile();

protected:

	void updateCompilerPath();

	bool execCompiler(CWorkSpaceItem * targetItem, bool buildAfterRun);

public slots:

	// シンボルの取得完了
	void listedSymbolsFinished(int exitCode, QProcess::ExitStatus exitStatus);

	// プロジェクトのビルド完了
	void buildFinished(int exitCode, QProcess::ExitStatus exitStatus);

	// ビルド中の出力を取得
	void buildReadOutput();

signals:

	void buildStart(const QString & filePath);
	void buildFinished(bool status);
	void buildOutput(const QString & output);

};

#endif // !defined(INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29)
