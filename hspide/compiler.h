#include <QObject>
#include <QProcess>

#ifndef INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29
#define INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29

class CProject;

class CCompiler
	: public QObject
{
	Q_OBJECT

	QProcess *mProcess;

public:

	CCompiler(QObject *parent = 0);

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
