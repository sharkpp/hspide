#include <QObject>
#include <QProcess>
#include <QString>

#ifndef INCLUDE_GUARD_DE12DB44_EA81_45D1_9E18_9CCA26479DE7
#define INCLUDE_GUARD_DE12DB44_EA81_45D1_9E18_9CCA26479DE7

class CProject
	: public QObject
{
	Q_OBJECT

public:

	typedef struct {
		QString	compilerPath;
	} Config;

private:

	Config	mConfig;

public:

	CProject(const CProject::Config & config);

	~CProject();

	// プロジェクトを読み込み
	bool load();

	// プロジェクトを保存
	bool save(const QString & filename = QString());

	// プロジェクトにファイルを追加
	bool append(const QString & filename);

	// プロジェクトから除外
	bool remove(const QString & filename);

	// プロジェクト内のファイルをメモリに読み込み
	bool openFile(const QString & filename);

	// プロジェクト内のファイルを保存
	bool saveFile(const QString & filename);

	// プロジェクト内のファイルを閉じる
	bool closeFile(const QString & filename);

	// プロジェクトをビルド
	void build();

public slots:

	// プロジェクトのビルド完了
	void buildFinished(int exitCode, QProcess::ExitStatus exitStatus);

signals:

	void buildStart();
	void buildFinished(bool status);

private:

	// プロジェクト内のファイルを一時的に保存
	bool saveTemp();

	// プロジェクト内の一次保存ファイルをクリア
	bool clearTemp();

};

#endif // defined(INCLUDE_GUARD_DE12DB44_EA81_45D1_9E18_9CCA26479DE7)
