#include <QProcess>
#include "projectitem.h"

#ifndef INCLUDE_GUARD_DE12DB44_EA81_45D1_9E18_9CCA26479DE7
#define INCLUDE_GUARD_DE12DB44_EA81_45D1_9E18_9CCA26479DE7

class CEditor;

class CProject
	: public QObject
	, public CProjectItem
{
	Q_OBJECT

public:

	typedef struct {
		QString	compilerPath;
	} Config;

private:

	Config	mConfig;
	QProcess *mProcess;

public:

	CProject(QObject *parent, const CProject::Config & config);

	virtual ~CProject();

	// プロジェクトを読み込み
	bool load(const QString & filename);

	// プロジェクトを保存
	bool save(const QString & filename = QString());

	// プロジェクトにファイルを追加
	bool append(const QString & filename);

	// プロジェクトから除外
	bool remove(const QString & filename);

	// プロジェクトへエディタを関連付け
	bool openFile(CEditor* editor);

	// プロジェクト内のファイルを閉じる
	bool closeFile(CEditor* editor);

// コンパイラ？クラスに移動予定
/*
	// プロジェクトをビルド
	void build();

public slots:

	// プロジェクトのビルド完了
	void buildFinished(int exitCode, QProcess::ExitStatus exitStatus);

	// ビルド中の出力を取得
	void buildReadOutput();

signals:

	void buildStart();
	void buildFinished(bool status);
	void buildOutput(const QString & output);
*/

private:

	// プロジェクト内のファイルを一時的に保存
	bool saveTemp();

	// プロジェクト内の一次保存ファイルをクリア
	bool clearTemp();

};

#endif // !defined(INCLUDE_GUARD_DE12DB44_EA81_45D1_9E18_9CCA26479DE7)
