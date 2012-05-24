#include <QObject>
#include <QMap>
#include <QSet>
#include <QUuid>
#include <QDebug>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

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

	// UUID�ƍs�ԍ����w�肵�ău���[�N�|�C���g��ǉ�
	bool append(const QUuid& uuid, int lineNumber);

	// UUID�ƍs�ԍ����w�肵�ău���[�N�|�C���g��ǉ�
	bool append(const QUuid& uuid, const QSet<int>& lineNumberes);

	const ListType& list() const;

	// UUID���w�肵�ēo�^�폜
	bool remove(const QUuid& uuid);

	// UUID�ƍs�ԍ����w�肵�ău���[�N�|�C���g�폜
	bool remove(const QUuid& uuid, int lineNumber);
	
	// �S�ēo�^�폜
	bool removeAll();
	
	// �u���[�N�|�C���g�����݂��Ă��邩
	bool isSet(const QUuid& uuid, int lineNumber) const;

private:

};

QDebug&      operator<<(QDebug& debug,       const BreakPointManager& v);
QDataStream& operator<<(QDataStream& stream, const BreakPointManager& v);
QDataStream& operator>>(QDataStream& stream,       BreakPointManager& v);
