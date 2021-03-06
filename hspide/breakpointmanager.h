#include <QObject>
#include <QMap>
#include <QSet>
#include <QUuid>
#include <QDebug>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef INCLUDE_GUARD_5C4A653F_1C27_4DBB_A7E2_601FB7A6FA8D
#define INCLUDE_GUARD_5C4A653F_1C27_4DBB_A7E2_601FB7A6FA8D

class BreakPointManager
	: public QObject
{
public:

	typedef QMap<QUuid, QSet<int> > ListType;

private:

	Q_OBJECT

	friend QDebug&      operator<<(QDebug& debug,       const BreakPointManager& v);
	friend QDataStream& operator<<(QDataStream& stream, const BreakPointManager& v);
	friend QDataStream& operator>>(QDataStream& stream,       BreakPointManager& v);

	ListType m_breakPointList;

public:

	BreakPointManager();
	~BreakPointManager();

	// UUIDと行番号を指定してブレークポイントを追加
	bool append(const QUuid& uuid, int lineNumber);

	// UUIDと行番号を指定してブレークポイントを追加
	bool append(const QUuid& uuid, const QSet<int>& lineNumberes);

	const ListType& list() const;

	// UUIDを指定して登録削除
	bool remove(const QUuid& uuid);

	// UUIDと行番号を指定してブレークポイント削除
	bool remove(const QUuid& uuid, int lineNumber);

	// 全て登録削除
	bool removeAll();

	// ブレークポイントが存在しているか
	bool isSet(const QUuid& uuid, int lineNumber) const;

	// ブレークポイントの個数
	int countOf(const QUuid& uuid) const;

private:

signals:

	void breakPointChanged(const QUuid& uuid, const QList<QPair<int, bool> >& modifiedLineNumberes);
};

QDebug&      operator<<(QDebug& debug,       const BreakPointManager& v);
QDataStream& operator<<(QDataStream& stream, const BreakPointManager& v);
QDataStream& operator>>(QDataStream& stream,       BreakPointManager& v);

#endif // !defined(INCLUDE_GUARD_5C4A653F_1C27_4DBB_A7E2_601FB7A6FA8D)
