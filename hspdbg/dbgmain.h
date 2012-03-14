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

	class typeinfo_hook
	{
		typedef spplib::thunk_generator<typeinfo_hook> thunk_type;
		thunk_type    m_cmdfunc_thunk;
		thunk_type    m_reffunc_thunk;
		thunk_type    m_termfunc_thunk;
		thunk_type    m_msgfunc_thunk;
		thunk_type    m_eventfunc_thunk;
		HSP3TYPEINFO* m_typeinfo;
		HSP3TYPEINFO  m_typeinfo_old;
	private:
		static int CALLBACK cmdfunc_thunk(int cmd) { return thunk_type::thunk->cmdfunc(cmd); }
		       int CALLBACK cmdfunc      (int cmd);
		static void* CALLBACK reffunc_thunk(int *type_res, int arg) { return thunk_type::thunk->reffunc(type_res, arg); }
		       void* CALLBACK reffunc      (int *type_res, int arg);
		static int CALLBACK termfunc_thunk(int option) { return thunk_type::thunk->termfunc(option); }
		       int CALLBACK termfunc      (int option);
		static int CALLBACK msgfunc_thunk(int prm1,int prm2,int prm3) { return thunk_type::thunk->msgfunc(prm1,prm2,prm3); }
		       int CALLBACK msgfunc      (int prm1,int prm2,int prm3);
		static int CALLBACK eventfunc_thunk(int event, int prm1, int prm2, void *prm3) { return thunk_type::thunk->eventfunc(event, prm1, prm2, prm3); }
		       int CALLBACK eventfunc      (int event, int prm1, int prm2, void *prm3);
		static int CALLBACK sballoc_thunk(int event, int prm1, int prm2, void *prm3) { return thunk_type::thunk->eventfunc(event, prm1, prm2, prm3); }
		       int CALLBACK sballoc      (int event, int prm1, int prm2, void *prm3);
	public:
		typeinfo_hook();
		void install_hook(HSP3TYPEINFO* typeinfo);
	};

	typeinfo_hook*	m_typeinfo_hook;

	typedef spplib::thunk_generator<CDbgMain> thunk_type;
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

	static char * CALLBACK sbAlloc_thunk(int size) { return thunk_type::thunk->sbAlloc(size); }
	       char * CALLBACK sbAlloc      (int size);
	static char * CALLBACK sbExpand_thunk(char *ptr, int size) { return thunk_type::thunk->sbExpand(ptr, size); }
	       char * CALLBACK sbExpand      (char *ptr, int size);

public slots:

	void connected();
	void recvCommand();
};
