#include <QObject>
#include <QProcess>

#ifndef INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29
#define INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29

class CProject;

class CCompiler
	: public QObject
{
	Q_OBJECT

	QString mHspCompPath;
	QString mHspPath;
	QString mHspCommonPath;

	QProcess *mProcess;

public:

	CCompiler(QObject *parent = 0);

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

	// プロジェクトをビルド
	void build(CProject * project);

	// 単一ファイルをコンパイル
	void compile();

protected:

public slots:

	// プロジェクトのビルド完了
	void buildFinished(int exitCode, QProcess::ExitStatus exitStatus);

	// ビルド中の出力を取得
	void buildReadOutput();

signals:

	void buildStart();
	void buildFinished(bool status);
	void buildOutput(const QString & output);

};

#endif // !defined(INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29)
