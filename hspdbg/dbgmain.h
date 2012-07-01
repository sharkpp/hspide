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
#include "../hspide/filemanager.h"
#include "../hspide/breakpointmanager.h"
#include <spplib/thunk.hpp>
#include "hspsdk/hsp3struct.h"

struct HSP3DEBUG;
struct HSPCTX;

class CDbgMain
	: public QCoreApplication
{
	Q_OBJECT

	static HANDLE	m_handle;
	static HANDLE	m_waitThread;
	static HANDLE	m_waitDisconnect;

	QLocalSocket*	m_socket;
	QString			m_id;

	QMutex			m_lock;

	HSP3DEBUG*		m_dbg;

	bool			m_breaked;	// デバッグの一時中断中か？
	bool			m_breakedLast; // 前回の状態
	bool			m_resumed;	// デバッグの再開直後か？
	bool			m_stepIn;	// デバッグの再開直後か？
	bool			m_quit;		// 終了要求が出ているか？

	BreakPointManager m_bp;
	FileManager		m_lookup;

	QByteArray		m_cmdBuff;

	QVector<QString> m_varNames;

	class typeinfo_hook
	{
		spplib::thunk m_cmdfunc_thunk;
		spplib::thunk m_reffunc_thunk;
		spplib::thunk m_termfunc_thunk;
		spplib::thunk m_msgfunc_thunk;
		spplib::thunk m_eventfunc_thunk;
		HSP3TYPEINFO* m_typeinfo;
		HSP3TYPEINFO  m_typeinfo_old;
	private:
		int cmdfunc(int cmd);
		void* reffunc(int *type_res, int arg);
		int termfunc(int option);
		int msgfunc(int prm1,int prm2,int prm3);
		int eventfunc(int event, int prm1, int prm2, void *prm3);
	public:
		typeinfo_hook();
		~typeinfo_hook();
		void install_hook(HSP3TYPEINFO* typeinfo);
		void uninstall_hook();
	};

	typeinfo_hook*	m_typeinfo_hook;

public:

	CDbgMain();
	virtual ~CDbgMain();

	void initialize(HSP3DEBUG* dbg);

	HSP3DEBUG* debugInfo();

	void hook(HSP3TYPEINFO* top, HSP3TYPEINFO* last);

	void connectToDebugger();
	void disconnectFromDebugger();
	void putLog(const char* text, int len);

	void updateInfo();

	bool isQuitRequested();

	bool isDebugging();

	bool isDebugPaused();
	bool isDebugResumed();
	void debugSuspend();
	void debugResume();

	bool isBreak(const char* filename, int lineNo);

	void breakRunning(const QUuid& uuid, int lineNo);

	static void create();
	static void destroy();

	int MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);

protected:

	static unsigned __stdcall runStatic(void* this_);

	static int WINAPI HookMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);

	void run();

	void updateDebugInfo();
	void updateVarInfo();

	bool GetVariableInfo(const QString& varName, int indexOf[], VARIABLE_INFO_TYPE& varInfo);
	bool GetVariableInfo(int indexOfVariable, int indexOf[], VARIABLE_INFO_TYPE& varInfo);

	void ReqestVariableInfo(const QString& varName, int indexOf[]);

	void initializeVariableNames();
	QString getVariableName(int index);

	QString loadString(HSPCTX* hspctx, int offset, bool allow_minus_idx);

	void installHook();

public slots:

	void connected();
	void disconnected();
	void disconnectLater();
	void recvCommand();
};
