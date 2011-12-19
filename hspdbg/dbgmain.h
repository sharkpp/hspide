#include <QCoreApplication>
#include <QThread>
#include <QLocalSocket>
#include "../hspide/debuggercommand.hpp"

class CDbgMain
	: public QThread
{
	Q_OBJECT

	QCoreApplication* m_app;

	QLocalSocket*	m_socket;
	long long		m_id;

	CDebuggerCommand m_cmdQueue;

public:
	CDbgMain();
	virtual ~CDbgMain();

	void connectToDebugger();
	void putLog(const char *text, int len);

protected:

	void run();

public slots:

	void connected();
	void recvCommand();
};
