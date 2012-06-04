#include "breakpointmanager.h"

QDebug& operator<<(QDebug& debug, const BreakPointManager& v)
{
	debug << v.m_breakPointList;
	return debug;
}

QDataStream& operator<<(QDataStream& stream, const BreakPointManager& v)
{
	stream << v.m_breakPointList;
	return stream;
}

QDataStream& operator>>(QDataStream& stream, BreakPointManager& v)
{
	stream >> v.m_breakPointList;
	return stream;
}

BreakPointManager::BreakPointManager()
{
}

BreakPointManager::~BreakPointManager()
{
}

// UUID�ƍs�ԍ����w�肵�ău���[�N�|�C���g��ǉ�
bool BreakPointManager::append(const QUuid& uuid, int lineNumber)
{
	m_breakPointList[uuid].insert(lineNumber);
	// �ύX��ʒm
	QList<QPair<int, bool> > modified;
	modified << qMakePair(lineNumber, true);
	emit breakPointChanged(uuid, modified);
	return true;
}

// UUID�ƍs�ԍ����w�肵�ău���[�N�|�C���g��ǉ�
bool BreakPointManager::append(const QUuid& uuid, const QSet<int>& lineNumberes)
{
	m_breakPointList[uuid].unite(lineNumberes);
	// �ύX��ʒm
	QList<QPair<int, bool> > modified;
	foreach(int lineNumber, lineNumberes) {
		modified << qMakePair(lineNumber, true);
	}
	emit breakPointChanged(uuid, modified);
	return true;
}

const BreakPointManager::ListType& BreakPointManager::list() const
{
	return m_breakPointList;
}

// UUID���w�肵�ēo�^�폜
bool BreakPointManager::remove(const QUuid& uuid)
{
	QList<QPair<int, bool> > modified;
	ListType::iterator
		ite = m_breakPointList.find(uuid);
	if( m_breakPointList.end() != ite ) {
		foreach(int lineNumber, ite->values()) {
			modified << qMakePair(lineNumber, false);
		}
	}
	// �폜
	m_breakPointList.remove(uuid);
	// �ύX��ʒm
	emit breakPointChanged(uuid, modified);
	return true;
}

// UUID�ƍs�ԍ����w�肵�ău���[�N�|�C���g�폜
bool BreakPointManager::remove(const QUuid& uuid, int lineNumber)
{
	ListType::iterator
		ite = m_breakPointList.find(uuid);
	if( m_breakPointList.end() != ite ) {
		ite->remove(lineNumber);
		if( ite->empty() ) {
			m_breakPointList.erase(ite);
		}
		// �ύX��ʒm
		QList<QPair<int, bool> > modified;
		modified << qMakePair(lineNumber, false);
		emit breakPointChanged(uuid, modified);
		return true;
	}
	return false;
}

// �S�ēo�^�폜
bool BreakPointManager::removeAll()
{
	ListType breakPointList = m_breakPointList;
	m_breakPointList.clear();

	foreach(QUuid uuid, breakPointList.keys()) {
		QList<QPair<int, bool> > modified;
		foreach(int lineNumber, breakPointList[uuid].values()) {
			modified << qMakePair(lineNumber, false);
		}
		emit breakPointChanged(uuid, modified);
	}

	return true;
}

// �u���[�N�|�C���g�����݂��Ă��邩
bool BreakPointManager::isSet(const QUuid& uuid, int lineNumber) const
{
	ListType::iterator
		ite = m_breakPointList.find(uuid);
	if( m_breakPointList.end() != ite &&
		ite->end() != ite->find(lineNumber) )
	{
		return true;
	}
	return false;
}

// �u���[�N�|�C���g�̌�
int BreakPointManager::countOf(const QUuid& uuid) const
{
	ListType::iterator
		ite = m_breakPointList.find(uuid);
	if( m_breakPointList.end() != ite )
	{
		return ite->count();
	}
	return -1;
}
