#include <QCoreApplication>
#include <QLocalSocket>

class CDbgMain
	: public QCoreApplication
{
	Q_OBJECT

	QLocalSocket*	m_socket;
	long long		m_id;

public:
	CDbgMain();
	virtual ~CDbgMain();

	void connectToDebugger();
	void putLog(const char *text, int len);
};
