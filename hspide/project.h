#include "projectitem.h"

#ifndef INCLUDE_GUARD_DE12DB44_EA81_45D1_9E18_9CCA26479DE7
#define INCLUDE_GUARD_DE12DB44_EA81_45D1_9E18_9CCA26479DE7

class CEditor;

class CProject
	: public CProjectItem
{
	Q_OBJECT

private:


public:

	CProject(QObject *parent);

	virtual ~CProject();

	// プロジェクトを読み込み
	bool load(const QString & filename);

	// プロジェクトを保存
	bool save(const QString & filename = QString());

	// プロジェクトにファイルを追加
	bool append(const QString & filename, const QString & path = QString());

	// プロジェクトから除外
	bool remove(const QString & filename);

	// プロジェクトへエディタを関連付け
	bool openFile(CEditor* editor);

	// プロジェクト内のファイルを閉じる
	bool closeFile(CEditor* editor);

	// プロジェクト名称取得
	QString name() const;

	// フォルダを取得
	CProjectItem * getFolderItem(const QString & path, bool createAlways = false);

private:

	// プロジェクト内のファイルを一時的に保存
	bool saveTemp();

	// プロジェクト内の一次保存ファイルをクリア
	bool clearTemp();

};

#endif // !defined(INCLUDE_GUARD_DE12DB44_EA81_45D1_9E18_9CCA26479DE7)
