#include <windows.h>
#include <QCoreApplication>
#include <QThread>
#include <QMutex>
#include <QLocalSocket>
#include <QString>
#include <QMap>
#include <QSet>
#include <QUuid>
#include "../hspide/debuggercommand.hpp"

typedef QMap<QUuid, QSet<int> > CBreakPointInfo;
typedef QMap<QString, QUuid> CUuidLookupInfo;

class CDbgMain
	: public QCoreApplication
{
	Q_OBJECT

	static HANDLE	m_handle;
	static HANDLE	m_waitThread;

	QLocalSocket*	m_socket;
	long long		m_id;

	QMutex			m_lock;

	CBreakPointInfo	m_bp;
	CUuidLookupInfo	m_lookup;

	CDebuggerCommand m_cmdQueue;

public:
	CDbgMain();
	virtual ~CDbgMain();

	void connectToDebugger();
	void putLog(const char *text, int len);

	bool isBreak(const char* filename, int lineNo);

	static void create();
	static void destroy();

protected:

	static unsigned __stdcall runStatic(void* this_);

	void run();

public slots:

	void connected();
	void recvCommand();
};
