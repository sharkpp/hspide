#include <QObject>
#include <QString>
#include <QMap>
#include <QSharedPointer>

#ifndef INCLUDE_GUARD_3CBB7292_EDC6_4258_B45F_54C680152079
#define INCLUDE_GUARD_3CBB7292_EDC6_4258_B45F_54C680152079

class CProject;
class CEditor;

class CSolution
	: public QObject
{
	Q_OBJECT

public:

	typedef struct {
	} Config;

private:

	Config	mConfig;

	QMap<QString, QSharedPointer<CProject> >	mProjects;

public:

	CSolution(QObject *parent, const CSolution::Config & config);

	~CSolution();

	// ソリューションを読み込み
	bool load();

	// ソリューションを保存
	bool save(const QString & filename);

	// ソリューションにプロジェクトを追加
	bool append(const QString & filename = QString());

	// ソリューションからプロジェクトを除外
	bool remove(const QString & filename);

	// ソリューションへエディタを関連付け
	bool openFile(CEditor* editor);

	// ソリューションからエディタを解除
	bool closeFile(CEditor* editor);

	// ソリューション名を取得
	QString title();

	// ソリューション名を取得
	QString filename();

	// プロジェクト数を取得
	int count();

	// プロジェクトを取得
	const CProject & at(int index) const;

	// ソリューションのビルド
	void build();

	// ソリューションの実行
	void run();

public slots:

	// プロジェクトのビルド開始
	void buildStartProject();

	// プロジェクトのビルド完了
	void buildFinishedProject(bool successed);

signals:

	void buildStart();
	void buildFinished(bool successed);

};

#endif // defined(INCLUDE_GUARD_3CBB7292_EDC6_4258_B45F_54C680152079)
