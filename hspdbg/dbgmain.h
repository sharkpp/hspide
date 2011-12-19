#include <windows.h>
#include <QCoreApplication>
#include <QThread>
#include <QLocalSocket>
#include "../hspide/debuggercommand.hpp"

class CDbgMain
	: public QCoreApplication
{
	Q_OBJECT

	static HANDLE	m_handle;
	static HANDLE	m_waitThread;

	QLocalSocket*	m_socket;
	long long		m_id;

	CDebuggerCommand m_cmdQueue;

public:
	CDbgMain();
	virtual ~CDbgMain();

	void connectToDebugger();
	void putLog(const char *text, int len);

	static void create();
	static void destroy();

protected:

	static unsigned __stdcall runStatic(void* this_);

	void run();

public slots:

	void connected();
	void recvCommand();
};
