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

	// UUID�ƍs�ԍ����w�肵�ău���[�N�|�C���g��ǉ�
	bool append(const QUuid& uuid, int lineNumber);

	// UUID�ƍs�ԍ����w�肵�ău���[�N�|�C���g��ǉ�
	bool append(const QUuid& uuid, const QSet<int>& lineNumberes);

	const CBreakPointList& toList() const;

	// UUID���w�肵�ēo�^�폜
	bool remove(const QUuid& uuid);

	// UUID�ƍs�ԍ����w�肵�ău���[�N�|�C���g�폜
	bool remove(const QUuid& uuid, int lineNumber);
	
	// �S�ēo�^�폜
	bool removeAll();

private:

};

