#include <windows.h>
#include <QCoreApplication>
#include <QThread>
#include <QMutex>
#include <QLocalSocket>
#include <QString>
#include <QMap>
#include <QSet>
#include <QUuid>
#include "../hspide/ipc_common.h"
#include "thunk_generator.hpp"
#include "hspsdk/hsp3struct.h"

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
	qint64			m_id;

	QMutex			m_lock;

	HSP3DEBUG*		m_dbg;

	bool			m_breaked;

	CBreakPointInfo	m_bp;
	CUuidLookupInfo	m_lookup;

	QByteArray		m_cmdBuff;

	QVector<QString> m_varNames;

	class cmdfunc_hook
	{
		typedef spplib::thunk_generator<cmdfunc_hook> thunk_type;
		thunk_type m_thunk;
		int (*m_cmdfunc_old)(int);
		HSP3TYPEINFO* m_typeinfo;
	private:
		static int CALLBACK cmdfunc_thunk(int cmd);
		int CALLBACK cmdfunc(int cmd);
	public:
		cmdfunc_hook();
		void install_hook(HSP3TYPEINFO* typeinfo);
	};

	cmdfunc_hook*	m_cmdfunc_hook;

public:
	CDbgMain();
	virtual ~CDbgMain();

	void initialize(HSP3DEBUG* dbg);

	HSP3DEBUG* debugInfo();

	void hook(HSP3TYPEINFO* top, HSP3TYPEINFO* last);

	void connectToDebugger();
	void putLog(const char* text, int len);

	void updateInfo();

	bool isBreak(const char* filename, int lineNo);

	static void create();
	static void destroy();

protected:

	static unsigned __stdcall runStatic(void* this_);

	void run();

	void updateDebugInfo();
	void updateVarInfo();

	bool GetVariableInfo(const QString& varName, int indexOf[], VARIABLE_INFO_TYPE& varInfo);
	bool GetVariableInfo(int indexOfVariable, int indexOf[], VARIABLE_INFO_TYPE& varInfo);

	void ReqestVariableInfo(const QString& varName, int indexOf[]);

	void initializeVariableNames();
	QString getVariableName(int index);

	QString loadString(HSPCTX* hspctx, int offset, bool allow_minus_idx);

public slots:

	void connected();
	void recvCommand();
};
