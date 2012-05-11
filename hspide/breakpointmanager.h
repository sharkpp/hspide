#include <QObject>
#include <QMap>
#include <QSet>
#include <QUuid>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

typedef QMap<QUuid, QSet<int> > CBreakPointList;

class BreakPointManager
	: public QObject
{
	Q_OBJECT

	CBreakPointList m_breakPointList;

public:

	BreakPointManager();
	~BreakPointManager();

	// UUIDと行番号を指定してブレークポイントを追加
	bool append(const QUuid& uuid, int lineNumber);

	// UUIDと行番号を指定してブレークポイントを追加
	bool append(const QUuid& uuid, const QSet<int>& lineNumberes);

	const CBreakPointList& toList() const;

	// UUIDを指定して登録削除
	bool remove(const QUuid& uuid);

	// UUIDと行番号を指定してブレークポイント削除
	bool remove(const QUuid& uuid, int lineNumber);
	
	// 全て登録削除
	bool removeAll();

private:

};

