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

struct HSP3DEBUG;
struct HSPCTX;

class CDbgMain
	: public QCoreApplication
{
	Q_OBJECT

	static HANDLE	m_handle;
	static HANDLE	m_waitThread;

	QLocalSocket*	m_socket;
	long long		m_id;

	QMutex			m_lock;

	bool			m_breaked;

	CBreakPointInfo	m_bp;
	CUuidLookupInfo	m_lookup;

	CDebuggerCommand m_cmdQueue;

	QVector<QString> m_varNames;

public:
	CDbgMain();
	virtual ~CDbgMain();

	void initialize(HSP3DEBUG* dbg);

	void connectToDebugger();
	void putLog(const char* text, int len);

	void updateInfo(HSP3DEBUG* dbg);

	bool isBreak(const char* filename, int lineNo);

	static void create();
	static void destroy();

protected:

	static unsigned __stdcall runStatic(void* this_);

	void run();

	void updateDebugInfo(HSP3DEBUG* dbg);
	void updateVarInfo(HSP3DEBUG* dbg);

	void initializeVariableNames(HSP3DEBUG* dbg);
	QString getVariableName(int index);

	QString loadString(HSPCTX* hspctx, int offset, bool allow_minus_idx);

public slots:

	void connected();
	void recvCommand();
};
