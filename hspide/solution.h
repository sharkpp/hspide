//#include <QObject>
#include <QString>
#include <QMap>
#include <QSharedPointer>
//#include <QAbstractItemModel>
#include "projectitem.h"

#ifndef INCLUDE_GUARD_3CBB7292_EDC6_4258_B45F_54C680152079
#define INCLUDE_GUARD_3CBB7292_EDC6_4258_B45F_54C680152079

class CProject;
class CEditor;

class CSolution
	: public CProjectItem
{
	Q_OBJECT

public:

	CSolution(QObject *parent);

	virtual ~CSolution();

	// ソリューションを読み込み
	bool load(const QString & filename);

	// ソリューションを保存
	bool save(const QString & filename);

	// ソリューションにプロジェクトを追加
	CProject * append(const QString & filename = QString());

	// ソリューションからプロジェクトを除外
	bool remove(const QString & filename);

	// ソリューションへエディタを関連付け
	bool openFile(CEditor* editor);

	// ソリューションからエディタを解除
	bool closeFile(CEditor* editor);

	// ソリューション名を取得
	QString title() const;

	// ソリューション名を取得
	QString filename() const;

	// プロジェクト数を取得
	int count() const;

	// プロジェクトを取得
	const CProject & at(int index) const;

private:

};

#endif // !defined(INCLUDE_GUARD_3CBB7292_EDC6_4258_B45F_54C680152079)
