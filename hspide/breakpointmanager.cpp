#include "breakpointmanager.h"

BreakPointManager::BreakPointManager()
{
}

BreakPointManager::~BreakPointManager()
{
}

// UUIDと行番号を指定してブレークポイントを追加
bool BreakPointManager::append(const QUuid& uuid, int lineNumber)
{
	m_breakPointList[uuid].insert(lineNumber);
	return true;
}

// UUIDと行番号を指定してブレークポイントを追加
bool BreakPointManager::append(const QUuid& uuid, const QSet<int>& lineNumberes)
{
	m_breakPointList[uuid].unite(lineNumberes);
	return true;
}

const CBreakPointList& BreakPointManager::toList() const
{
	return m_breakPointList;
}

// UUIDを指定して登録削除
bool BreakPointManager::remove(const QUuid& uuid)
{
	m_breakPointList.remove(uuid);
	return true;
}

// UUIDと行番号を指定してブレークポイント削除
bool BreakPointManager::remove(const QUuid& uuid, int lineNumber)
{
	CBreakPointList::iterator
		ite = m_breakPointList.find(uuid);
	if( m_breakPointList.end() != ite ) {
		ite->remove(lineNumber);
		if( ite->empty() ) {
			m_breakPointList.erase(ite);
		}
		return true;
	}
	return false;
}

// 全て登録削除
bool BreakPointManager::removeAll()
{
	m_breakPointList.clear();
	return true;
}

