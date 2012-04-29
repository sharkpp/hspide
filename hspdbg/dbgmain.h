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
#include <spplib/thunk.hpp>
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

	class typeinfo_hook
	{
		typedef spplib::thunk thunk_type;
		thunk_type    m_cmdfunc_thunk;
		thunk_type    m_reffunc_thunk;
		thunk_type    m_termfunc_thunk;
		thunk_type    m_msgfunc_thunk;
		thunk_type    m_eventfunc_thunk;
		HSP3TYPEINFO* m_typeinfo;
		HSP3TYPEINFO  m_typeinfo_old;
	private:
		int cmdfunc(int cmd);
		void* reffunc(int *type_res, int arg);
		int termfunc(int option);
		int msgfunc(int prm1,int prm2,int prm3);
		int eventfunc(int event, int prm1, int prm2, void *prm3);
		int sballoc(int event, int prm1, int prm2, void *prm3);
	public:
		typeinfo_hook();
		void install_hook(HSP3TYPEINFO* typeinfo);
	};

	typeinfo_hook*	m_typeinfo_hook;

	typedef spplib::thunk thunk_type;
	thunk_type    m_sbAlloc_thunk;
	thunk_type    m_sbExpand_thunk;

	char * (*m_sbAlloc)(int size);
	char * (*m_sbExpand)(char *ptr, int size);

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

	char * sbAlloc(int size);
	char * sbExpand(char *ptr, int size);

public slots:

	void connected();
	void recvCommand();
};
