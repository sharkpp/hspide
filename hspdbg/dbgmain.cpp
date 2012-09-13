#include <QTextCodec>
#include <QStringList>
#include <process.h>
#include <tchar.h>
#include "dbgmain.h"
#include <spplib/apihook.h>

#if defined(_UNICODE) || defined(UNICODE)
#undef LOGFONT
#define LOGFONT LOGFONTA
#endif

#include "hspsdk/hsp3debug.h"
#include "hspsdk/hsp3struct.h"
#include "hspsdk/hspwnd.h"

#if defined(_UNICODE) || defined(UNICODE)
#undef LOGFONT
#define LOGFONT LOGFONTW
#endif

extern CDbgMain* g_app;

// dummy argument for QCoreApplication
static int argc_ = 1;
static char *argv_[] = {""};

typedef enum {
	DINFO_TYPE_DATA_END = 255,			// �f�[�^��̏I�[
	DINFO_TYPE_SOURCE_FILE_NAME = 254,	// �\�[�X�t�@�C���w��
	DINFO_TYPE_VARIABLE = 253,			// �ϐ����f�[�^�w��
	DINFO_TYPE_OFFSET = 252,			// ����16bit�l���A���s�܂ł�CS�I�t�Z�b�g
} DINFO_TYPE;

HANDLE	CDbgMain::m_handle = NULL;
HANDLE	CDbgMain::m_waitThread = NULL;
HANDLE	CDbgMain::m_waitDisconnect = NULL;

// �t�b�N���s��API�̃I���W�i����ۑ����Ă����ϐ�
static int (WINAPI * MessageBoxA_)(HWND,LPCSTR,LPCSTR,UINT) = NULL;

//--------------------------------------------------------------------
// typeinfo_hook�N���X

CDbgMain::typeinfo_hook::typeinfo_hook()
	: m_cmdfunc_thunk  (this, &CDbgMain::typeinfo_hook::cmdfunc) // C4355
	, m_reffunc_thunk  (this, &CDbgMain::typeinfo_hook::reffunc) // C4355
	, m_termfunc_thunk (this, &CDbgMain::typeinfo_hook::termfunc) // C4355
	, m_msgfunc_thunk  (this, &CDbgMain::typeinfo_hook::msgfunc) // C4355
	, m_eventfunc_thunk(this, &CDbgMain::typeinfo_hook::eventfunc) // C4355
	, m_typeinfo(NULL)
{
}

CDbgMain::typeinfo_hook::~typeinfo_hook()
{
	uninstall_hook();
}

void CDbgMain::typeinfo_hook::install_hook(HSP3TYPEINFO* typeinfo)
{
	if( !m_typeinfo ) {
		m_typeinfo    = typeinfo;
		m_typeinfo_old= *typeinfo;
		// cmdfunc�Ȃǂ̃t�b�N
		if( typeinfo->cmdfunc ) {
			typeinfo->cmdfunc = (int(*)(int))m_cmdfunc_thunk.get_code();
		}
		if( typeinfo->reffunc ) {
			typeinfo->reffunc = (void*(*)(int*,int))m_reffunc_thunk.get_code();
		}
		if( typeinfo->termfunc ) {
			typeinfo->termfunc = (int(*)(int))m_termfunc_thunk.get_code();
		}
		//if( typeinfo->msgfunc ) {
		//	m_typeinfo_old.msgfunc = (int(*)(int,int,int))m_msgfunc_thunk.injection_code(typeinfo->msgfunc);
		//}
		if( typeinfo->eventfunc ) {
			typeinfo->eventfunc = (int(*)(int,int,int,void*))m_eventfunc_thunk.get_code();
		}
	}
}

void CDbgMain::typeinfo_hook::uninstall_hook()
{
	if( m_typeinfo ) {
		if( m_typeinfo->cmdfunc ) {
			m_typeinfo->cmdfunc = m_typeinfo_old.cmdfunc;
		}
		if( m_typeinfo->reffunc ) {
			m_typeinfo->reffunc = m_typeinfo_old.reffunc;
		}
		if( m_typeinfo->termfunc ) {
			m_typeinfo->termfunc = m_typeinfo_old.termfunc;
		}
		if( m_typeinfo->msgfunc ) {
		//	m_msgfunc_thunk.uninjection_code(m_typeinfo->msgfunc, m_typeinfo_old.msgfunc);
		}
		if( m_typeinfo->eventfunc ) {
			m_typeinfo->eventfunc = m_typeinfo_old.eventfunc;
		}
		m_typeinfo = NULL;
	}
}

int CDbgMain::typeinfo_hook::cmdfunc(int cmd)
{
	if( g_app->isDebugging() )
	{
		return 0;
	}

#ifdef _DEBUG
	HSP3DEBUG* dbg = g_app->debugInfo();
	dbg->dbg_curinf();
	char tmp[256];
	sprintf(tmp,"%p %s(%2d)/%2d>>%s(%2d)",this,__FUNCTION__,cmd,m_typeinfo_old.type,dbg->fname?dbg->fname:"???",dbg->line);
	g_app->putLog(tmp, strlen(tmp));
#endif

	return m_typeinfo_old.cmdfunc(cmd);
}

void* CDbgMain::typeinfo_hook::reffunc(int *type_res, int arg)
{
#ifdef _DEBUG
	HSP3DEBUG* dbg = g_app->debugInfo();
	dbg->dbg_curinf();
	char tmp[256];
	sprintf(tmp,"%p %s(%p,%d)/%2d>>%s(%2d)",this,__FUNCTION__,type_res,arg,m_typeinfo->type,dbg->fname?dbg->fname:"???",dbg->line);
	g_app->putLog(tmp, strlen(tmp));
#endif

	return m_typeinfo_old.reffunc(type_res, arg);
}

int CDbgMain::typeinfo_hook::termfunc(int option)
{
OutputDebugStringA("typeinfo_hook::termfunc #1\n");
	if( g_app ) {
#ifdef _DEBUG
		HSP3DEBUG* dbg = g_app->debugInfo();
		dbg->dbg_curinf();
		char tmp[256];
		sprintf(tmp,"%p %s(%d)/%2d>>%s(%2d)",this,__FUNCTION__,option,m_typeinfo->type,dbg->fname?dbg->fname:"???",dbg->line);
OutputDebugStringA(tmp);
		g_app->putLog(tmp, strlen(tmp));
#endif
	}
OutputDebugStringA("typeinfo_hook::termfunc #2\n");

	return m_typeinfo_old.termfunc(option);
}

int CDbgMain::typeinfo_hook::msgfunc(int prm1,int prm2,int prm3)
{
#ifdef _DEBUG
	HSP3DEBUG* dbg = g_app->debugInfo();
	dbg->dbg_curinf();
	char tmp[256];
	sprintf(tmp,"%p %s(%d,%d,%d)/%2d>>%s(%2d)",this,__FUNCTION__,prm1,prm2,prm3,m_typeinfo->type,dbg->fname?dbg->fname:"???",dbg->line);
	g_app->putLog(tmp, strlen(tmp));
#endif

	return m_typeinfo_old.msgfunc(prm1,prm2,prm3);
}

int CDbgMain::typeinfo_hook::eventfunc(int event, int prm1, int prm2, void *prm3)
{
#ifdef _DEBUG
	HSP3DEBUG* dbg = g_app->debugInfo();
	dbg->dbg_curinf();
	char tmp[256];
	sprintf(tmp,"%p %s(%d,%d,%d,%p)/%2d>>%s(%2d)",this,__FUNCTION__,event,prm1,prm2,prm3,m_typeinfo->type,dbg->fname?dbg->fname:"???",dbg->line);
	g_app->putLog(tmp, strlen(tmp));
#endif

	return m_typeinfo_old.eventfunc(event,prm1,prm2,prm3);
}

//--------------------------------------------------------------------
// CDbgMain�N���X

CDbgMain::CDbgMain()
	: QCoreApplication(argc_, argv_)
	, m_socket(new QLocalSocket(this))
	, m_dbg(NULL)
	, m_breaked(false)
	, m_breakedLast(false)
	, m_resumed(false)
	, m_stepIn(false)
	, m_quit(false)
	, m_typeinfo_hook(NULL)
{
	// ��M�ʒm�Ȃǂ̃V�O�i���Ɛڑ�
	connect(m_socket, SIGNAL(connected()),    this, SLOT(connected()));
	connect(m_socket, SIGNAL(readyRead()),    this, SLOT(recvCommand()));
	connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(m_socket, SIGNAL(disconnected()), this, SLOT(quit()));
//	connect(m_socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));

	// �t�H�[�}�b�g��
	// hspide.{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}.{YYYYYYYY-YYYY-YYYY-YYYY-YYYYYYYYYYYY}
	//        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//        �T�[�o�[GUID                           �A�C�e��GUID

#if __STDC_WANT_SECURE_LIB__
	TCHAR* envVar;
	size_t envSize;
	_tgetenv_s(&envSize, NULL, 0, "hspide");
	envVar = new TCHAR[envSize];
	_tgetenv_s(&envSize, envVar, envSize, "hspide" );
	QStringList ideInfo = QString(envVar).split(".");
	delete [] envVar;
#else
	QStringList ideInfo = QString(getenv("hspide")).split(".");
#endif

	m_id = (3 == ideInfo.size() ? ideInfo[2] : "");

	// �f�o�b�K�T�[�o�[�ɐڑ�
	QString serverName = (3 == ideInfo.size() ? QString("hspide.%1").arg(ideInfo[1]) : "");
	m_socket->connectToServer(serverName);

	// �ڑ��ʒm���M
	connectToDebugger();

	m_typeinfo_hook = new typeinfo_hook[1024];

	installHook();
}

CDbgMain::~CDbgMain()
{
	delete [] m_typeinfo_hook;

	if( m_socket->isOpen() ) {
		m_socket->disconnectFromServer();
	}
}

void CDbgMain::create()
{
	// �ʒm�C�x���g������
	m_waitThread     = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_waitDisconnect = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	// �X���b�h�J�n
	m_handle
		= (HANDLE)::_beginthreadex(NULL, 0, CDbgMain::runStatic,
		                           (void*)&g_app, 0, NULL);
	// �����������܂őҋ@
	::WaitForSingleObject(m_waitThread, INFINITE);
}

void CDbgMain::destroy()
{
	if( !g_app )
	{
		return;
	}
OutputDebugStringA("CDbgMain::destroy #1\n");
	// �I���v��
	//QMetaObject::invokeMethod(g_app, "quit");
	g_app->disconnectFromDebugger();
OutputDebugStringA("CDbgMain::destroy #2\n");
	// �I���ҋ@
	DWORD dwResult = ::WaitForSingleObject(m_waitThread, 1000);
OutputDebugStringA("CDbgMain::destroy #3\n");
	// ���\�[�X���
	::CloseHandle(m_waitDisconnect);
	::CloseHandle(m_waitThread);
	::CloseHandle(m_handle);
	// �����I��
	if( WAIT_TIMEOUT == dwResult ) {
		::TerminateProcess(m_waitThread, -1);
	}
}

unsigned CDbgMain::runStatic(void* p)
{
	CDbgMain* app
		= new CDbgMain();
	*static_cast<CDbgMain**>(p) = app;

	// ���C������
	app->exec();
OutputDebugStringA("CDbgMain::runStatic #1\n");

	// �ؒf�ҋ@
	::WaitForSingleObject(m_waitDisconnect, INFINITE);
OutputDebugStringA("CDbgMain::runStatic #2\n");

	*static_cast<CDbgMain**>(p) = NULL;
OutputDebugStringA("CDbgMain::runStatic #3\n");

	delete app;
OutputDebugStringA("CDbgMain::runStatic #4\n");

	// �I��������ʒm
	::SetEvent(m_waitThread);

	return 0;
}

void CDbgMain::installHook()
{
	// �I���W�i����API��ۑ�
	if( !MessageBoxA_ ) {
		MessageBoxA_ = ::MessageBoxA;
	}

	INSTALL_HOOK("user32.dll", "MessageBoxA", HookMessageBoxA, NULL);
}

int CDbgMain::HookMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	return g_app->MessageBoxA(hWnd, lpText, lpCaption, uType);
}

int CDbgMain::MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	if( 0 != lstrcmpiA("Error", lpCaption) ||
		(MB_ICONEXCLAMATION|MB_OK) != uType )
	{
		return MessageBoxA_(hWnd, lpText, lpCaption, uType);
	}

	HSP3DEBUG* dbg = g_app->debugInfo();
	HSPCTX*    ctx = (HSPCTX*)dbg->hspctx;

	if( 0 == ctx->err )
	{
		return MessageBoxA_(hWnd, lpText, lpCaption, uType);
	}

	// �G���[�Œ��f

	putLog(lpText, lstrlenA(lpText));

	QString fname = dbg->fname ? dbg->fname : "";
	QUuid uuid;

	if( '?' == fname[0] )
	{
		// UUID���ڎw��
		uuid = QUuid(fname.mid(1));
	}
	else
	{
		// ���O����
		uuid = m_lookup.uuidFromFilename(fname);
	}
	if( !uuid.isNull() )
	{
		updateInfo();
		breakRunning(uuid, dbg->line);
	}

	::WaitForSingleObject(m_waitDisconnect, INFINITE);

	return 0;
}

void CDbgMain::connectToDebugger()
{
	QByteArray  data;
	QDataStream out(&data, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_4);
	out << m_id;
	IpcSend(*m_socket, CMD_CONNECT, data);
}

void CDbgMain::disconnectFromDebugger()
{
	QMetaObject::invokeMethod(this, "disconnectLater");
}

void CDbgMain::putLog(const char *text, int len)
{
	if( !isQuitRequested() ) {
		QByteArray  data(text, len);
		IpcSend(*m_socket, CMD_PUT_LOG, data);
	}
}

// �I���v�����o�Ă��邩�H
bool CDbgMain::isQuitRequested()
{
	QMutexLocker lck(&m_lock);
	return m_quit;
}

bool CDbgMain::isDebugging()
{
	HSP3DEBUG* dbg = debugInfo();
	HSPCTX*    ctx = (HSPCTX*)dbg->hspctx;

	dbg->dbg_curinf();

	// �u���[�N�|�C���g����H
	bool breaked = isBreak(dbg->fname, dbg->line);

	QMutexLocker lck(&m_lock);

	breaked |= m_breaked;

	// �ĊJ�w���H
	if( m_resumed )
	{
		m_breaked = false;
		m_resumed = false;
		m_breakedLast = false;
		return false;
	}

	// �X�e�b�vIN���w�����ꂽ�ꍇ�͈��̂݃R�[�h�����s
	if( m_stepIn )
	{
		m_stepIn  = false;
		m_breaked = true;
		m_breakedLast = false;
		return false;
	}

	// �ꎞ��~���̏ꍇ�̓R�[�h��ǂݎ�炸�ɖ߂�
	if( breaked )
	{
		// �I���v�����o�Ă��邩�H
		if( m_quit )
		{
			ctx->endcode = 0;
			ctx->runmode = RUNMODE_END;
			return true; // return 1;
		}
		//
		if( breaked != m_breakedLast )
		{
			m_breakedLast = breaked;

			QString fname = dbg->fname ? dbg->fname : "";
			QUuid uuid;

			if( '?' == fname[0] )
			{
				// UUID���ڎw��
				uuid = QUuid(fname.mid(1));
			}
			else
			{
				// ���O����
				uuid = m_lookup.uuidFromFilename(fname);
			}
			if( !uuid.isNull() )
			{
				lck.unlock(); // �f�b�h���b�N����̂ł����Ń��b�N������
				updateInfo();
				breakRunning(uuid, dbg->line);
				return true;
			}
		}
		lck.unlock();
		// ���邮����̂�CPU��D��Ȃ��悤�ɃX���[�v
		Sleep(100);
		return true; // return 0;
	}

	return false;
}

// �f�o�b�O�𒆒f
void CDbgMain::debugSuspend()
{
	QMutexLocker lck(&m_lock);
	m_breaked = true;
}

// �f�o�b�O���ĊJ
void CDbgMain::debugResume()
{
	QMutexLocker lck(&m_lock);
	m_breaked = false;
}

bool CDbgMain::isBreak(const char* filename, int lineNo)
{
	QMutexLocker lck(&m_lock);

	bool breaked = false;

	// �u���[�N�|�C���g�Ɉ��������邩���ׂ�
	QString fname = filename ? filename : "";
	QUuid uuid;

	if( '?' == fname[0] )
	{
		// UUID���ڎw��
		uuid = QUuid(fname.mid(1));
	}
	else
	{
		// ���O����
		uuid = m_lookup.uuidFromFilename(fname);
	}
	if( !uuid.isNull() )
	{
		if( m_bp.isSet(uuid, lineNo) )
		{
			breaked = true;
		}
	}

	if( breaked )
	{
		return true;
	}

	return false;
}

void CDbgMain::breakRunning(const QUuid& uuid, int lineNo)
{
	QByteArray  data;
	QDataStream out(&data, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_4);
	out << uuid << lineNo;
	IpcSend(*m_socket, CMD_STOP_RUNNING, data);
}

// data seg ���� �������ǂݍ���
QString CDbgMain::loadString(HSPCTX* hspctx, int offset, bool allow_minus_idx)
{
	QTextCodec* codec = QTextCodec::codecForLocale();
	QString r;

	size_t         ds_len = hspctx->hsphed->max_ds;
	unsigned char* ds_ptr = (unsigned char*)hspctx->mem_mds;

// ���䕶����\�������ɒu�����鏈��������
	if( allow_minus_idx && -1 == offset ) {
		r = "";
	} else if( offset < 0 || (int)ds_len <= offset ) {
		r = "<NULL>";
	} else {
		char * pStart = (char *)( ds_ptr + offset );
		char * pEnd   = pStart;
		char * pLast  = (char *)::memchr(pStart, 0, ds_len - offset);
		if( pLast ) {
			for(; pEnd < pLast ; pEnd++)
			{
				if( IsDBCSLeadByteEx(CP_ACP, (BYTE)*pEnd) ) {
					pEnd ++; // SJIS�S�p��1�o�C�g��
				} else {
					QString tmp;
					switch( *pEnd ) {
					case '\t': tmp = "\\t";		break;
					case '\\': tmp = "\\\\";	break;
					case '\"': tmp = "\\\"";	break;
					case 0x0d:
						if( 0x0a == *(pEnd + 1) ) {
							tmp = "\\n";
							r += codec->toUnicode(pStart, int(pEnd - pStart));
							r += tmp;
							pEnd++;
							pStart = pEnd + 1;
							continue;
						} else {
							tmp = "\\r";
						}
						break;
					default: continue;
					}
					r += codec->toUnicode(pStart, int(pEnd - pStart));
					r += tmp;
					pStart = pEnd + 1;
				}
			}
			// �c���ǉ�
			r += codec->toUnicode(pStart, int(pEnd - pStart));
		}
	}
	return r;
}

void CDbgMain::initializeVariableNames()
{
	HSPCTX* hspctx = (HSPCTX*)m_dbg->hspctx;

	size_t         di_len = hspctx->hsphed->max_dinfo;
	unsigned char* di_ptr = hspctx->mem_di;
	unsigned char* di_last= di_ptr + di_len;

	size_t         ds_len = hspctx->hsphed->max_ds;

	for(;di_ptr < di_last ; ) {
		switch( *di_ptr ) {
		case DINFO_TYPE_DATA_END:
			di_ptr++;
			break;
		case DINFO_TYPE_SOURCE_FILE_NAME:
			di_ptr += 4 + 2; // �t�@�C���� + �s�ԍ�
			break;
		case DINFO_TYPE_VARIABLE: {
			size_t offset = (size_t)((di_ptr[3] << 16) | (di_ptr[2] << 8) | di_ptr[1]);
			if( offset < ds_len ) {
				m_varNames.push_back(loadString(hspctx, offset, false));
			}
			di_ptr += 4;
			break; }
		case DINFO_TYPE_OFFSET:
			di_ptr += 4;
			break;
		default:
			di_ptr++;
		}
	}
}

QString CDbgMain::getVariableName(int index)
{
	if( 0 <= index &&
		index < m_varNames.size() )
	{
		return m_varNames[index];
	}
	return QString("v%1").arg(index);
}

void CDbgMain::initialize(HSP3DEBUG* dbg)
{
	m_dbg = dbg;

	initializeVariableNames();
}

HSP3DEBUG* CDbgMain::debugInfo()
{
	return m_dbg;
}

void CDbgMain::hook(HSP3TYPEINFO* top, HSP3TYPEINFO* last)
{
	for(HSP3TYPEINFO* ite = top; ite < last; ++ite)
	{
		// ��������
		if( ite->type == short(ite - top) ) {
			m_typeinfo_hook[ite - top].install_hook(ite);
		}
	}
}

void CDbgMain::unhook()
{
	for(int i = 0; i < 1024; i++)
	{
		m_typeinfo_hook[i].uninstall_hook();
	}
}

void CDbgMain::updateInfo()
{
	// �f�o�b�O���X�V
	updateDebugInfo();

	// �ϐ����擾
	updateVarInfo();
}

void CDbgMain::updateDebugInfo()
{
	QTextCodec* codec = QTextCodec::codecForLocale();

	if( !m_dbg->fname ) {
		return;
	}

	QVector<QPair<QString,QString> > debugInfo;

	int bmscr_max = -1; // �E�C���h�EId�̍ő��

	char* ptr = m_dbg->get_value(DEBUGINFO_GENERAL);

	for(const char* p = ptr; *p;)
	{
		const char* key = p;
		const char* value = p;
		for(; *p && 0x0D != p[0] && 0x0A != p[1]; ++value, ++p);
		if( 0x0D != *p ) { break; }
		value += 2;
		p += 2;
		for(; *p && 0x0D != p[0] && 0x0A != p[1]; ++p);
		if( 0x0D != *p ) { break; }
		p += 2;

		QString key_str   = codec->toUnicode(key,   int(value - 2 - key));
		QString value_str = codec->toUnicode(value, int(p - 2 - value));

		// �E�C���h�EId�̍ő�����擾
		if( !key_str.compare(codec->toUnicode("�E�C���h�D�ő�")) ) {
			bmscr_max = value_str.toInt();
		}
	//	debugInfo.append(qMakePair(key_str, value_str));
	}

	m_dbg->dbg_close(ptr);

	HSPCTX* hspctx = (HSPCTX*)m_dbg->hspctx;

	// ���݂̃E�C���h�EId���擾
	int actscr = hspctx->exinfo.actscr
					? *hspctx->exinfo.actscr
					: hspctx->exinfo2 && hspctx->exinfo2->actscr
						? *hspctx->exinfo2->actscr
						: -1;

	// �o�[�W�����ԍ�
	int hspver = hspctx->exinfo.ver | hspctx->exinfo.min;

	// ���݂̃E�C���h�EId���擾
	actscr = hspctx->exinfo.actscr
				? *hspctx->exinfo.actscr
				: hspctx->exinfo2 && hspctx->exinfo2->actscr
					? *hspctx->exinfo2->actscr
					: -1;

	// ���݂̃f�B���N�g�����擾
	char currentPath[MAX_PATH*2+1];
	::GetCurrentDirectoryA(MAX_PATH*2, currentPath);

	// ���s���
	static const char* RUNMODE[] = {
		"0:RUN",	"1:WAIT",	"2:AWAIT",	"3:STOP",
		"4:END",	"5:ERROR",	"6:RETURN",	"7:INTJUMP",
		"8:ASSERT",	"9:LOGMES",	"10:EXITRUN",
	};

	BMSCR*  bmscr  = (BMSCR*)(*hspctx->exinfo2->HspFunc_getbmscr)(0);

	// �f�o�b�O�����擾

	debugInfo
		<< qMakePair(codec->toUnicode("�V�X�e�����/�f�B���N�g��"),   QString("\"%1\"").arg(codec->toUnicode(currentPath)))
		<< qMakePair(codec->toUnicode("�V�X�e�����/�R�}���h���C��"), hspctx->cmdline ? QString("\"%1\"").arg(codec->toUnicode(hspctx->cmdline)) : QString(""))
		;

	if( bmscr )
	{
		debugInfo
			<< qMakePair(codec->toUnicode("�V�X�e�����/�E�B���h�E�ő�"),     QString("%1").arg(bmscr_max))
			<< qMakePair(codec->toUnicode("�V�X�e�����/�J�����g�E�B���h�E"), QString("%1").arg(actscr))
			;
	}

	debugInfo
		<< qMakePair(codec->toUnicode("�V�X�e�����/ax�T�C�Y"),     QString("%1").arg(hspctx->hsphed->allsize))
		<< qMakePair(codec->toUnicode("�V�X�e�����/�R�[�h�T�C�Y"), QString("%1").arg(hspctx->hsphed->max_cs))
		<< qMakePair(codec->toUnicode("�V�X�e�����/�f�[�^�T�C�Y"), QString("%1").arg(hspctx->hsphed->max_ds))
		<< qMakePair(codec->toUnicode("�V�X�e�����/�ϐ��\��"),     QString("%1").arg(hspctx->hsphed->max_val))
		<< qMakePair(codec->toUnicode("�V�X�e�����/���s���[�h"),   hspctx->runmode < 0 || RUNMODE_MAX <= hspctx->runmode ? QString("%1:�s��").arg(hspctx->runmode) : QString(RUNMODE[hspctx->runmode]))
		;

	if( bmscr )
	{
		debugInfo
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/flag"),               QString("%1").arg(bmscr->flag))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/sx"),                 QString("%1").arg(bmscr->sx))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/sy"),                 QString("%1").arg(bmscr->sy))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/palmode"),            QString("%1").arg(bmscr->palmode))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/hdc"),                QString("0x%1").arg((uint)bmscr->hdc,     8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/pBit"),               QString("0x%1").arg((uint)bmscr->pBit,    8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/pbi"),                QString("0x%1").arg((uint)bmscr->pbi,     8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/dib"),                QString("0x%1").arg((uint)bmscr->dib,     8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/old"),                QString("0x%1").arg((uint)bmscr->old,     8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/pal"),                QString("0x%1").arg((uint)bmscr->pal,     8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/hpal"),               QString("0x%1").arg((uint)bmscr->hpal,    8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/holdpal"),            QString("0x%1").arg((uint)bmscr->holdpal, 8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/pals"),               QString("%1").arg(bmscr->pals))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/hwnd"),               QString("0x%1").arg((uint)bmscr->hwnd,    8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/hInst"),              QString("0x%1").arg((uint)bmscr->hInst,   8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/infsize"),            QString("%1").arg(bmscr->infsize))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/bmpsize"),            QString("%1").arg(bmscr->bmpsize))
		// Window object setting
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/type"),               QString("%1").arg(bmscr->type))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/wid"),                QString("%1").arg(bmscr->wid))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/fl_dispw"),           QString("%1").arg(bmscr->fl_dispw))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/fl_udraw"),           QString("%1").arg(bmscr->fl_udraw))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/wx"),                 QString("%1").arg(bmscr->wx))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/wy"),                 QString("%1").arg(bmscr->wy))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/wchg"),               QString("%1").arg(bmscr->wchg))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/viewx"),              QString("%1").arg(bmscr->viewx))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/viewy"),              QString("%1").arg(bmscr->viewy))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/lx"),                 QString("%1").arg(bmscr->lx))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/ly"),                 QString("%1").arg(bmscr->ly))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/cx"),                 QString("%1").arg(bmscr->cx))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/cy"),                 QString("%1").arg(bmscr->cy))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/ox"),                 QString("%1").arg(bmscr->ox))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/oy"),                 QString("%1").arg(bmscr->oy))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/py"),                 QString("%1").arg(bmscr->py))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/texty"),              QString("%1").arg(bmscr->texty))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/gx"),                 QString("%1").arg(bmscr->gx))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/gy"),                 QString("%1").arg(bmscr->gy))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/gmode"),              QString("%1").arg(bmscr->gmode))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/hbr"),                QString("0x%1").arg((uint)bmscr->hbr,     8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/hpn"),                QString("0x%1").arg((uint)bmscr->hpn,     8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/hfont"),              QString("0x%1").arg((uint)bmscr->hfont,   8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/holdfon"),            QString("0x%1").arg((uint)bmscr->holdfon, 8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/color"),              QString("(%1,%2,%3)").arg(GetRValue(bmscr->color)).arg(GetGValue(bmscr->color)).arg(GetBValue(bmscr->color)))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/textspeed"),          QString("%1").arg(bmscr->textspeed))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/cx2"),                QString("%1").arg(bmscr->cx2))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/cy2"),                QString("%1").arg(bmscr->cy2))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/tex"),                QString("%1").arg(bmscr->tex))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/tey"),                QString("%1").arg(bmscr->tey))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/prtmes"),             QString("0x%1").arg((uint)bmscr->prtmes, 8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/focflg"),             QString("%1").arg(bmscr->focflg))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/objmode"),            QString("%1").arg(bmscr->objmode))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/logfont"),            QString("{ .lfHeight=%1, .lfFaceName=\"%2\" }").arg(bmscr->logfont.lfHeight).arg(codec->toUnicode(bmscr->logfont.lfFaceName)))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/logfont/lfHeight"),   QString("%1").arg(bmscr->logfont.lfHeight))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/logfont/lfFaceName"), QString("\"%1\"").arg(codec->toUnicode(bmscr->logfont.lfFaceName)))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/style"),              QString("%1").arg(bmscr->style))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/gfrate"),             QString("%1").arg(bmscr->gfrate))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/tabmove"),            QString("%1").arg(bmscr->tabmove))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/sx2"),                QString("%1").arg(bmscr->sx2))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/printsize"),          QString("{ .cx=%1, .cy=%2 }").arg(bmscr->printsize.cx).arg(bmscr->printsize.cy))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/printsize/cx"),       QString("%1").arg(bmscr->printsize.cx))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/printsize/cy"),       QString("%1").arg(bmscr->printsize.cy))
			// Class depend data
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/objstyle"),           QString("%1").arg(bmscr->objstyle))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/mem_obj"),            QString("0x%1").arg((uint)bmscr->mem_obj, 8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/objmax"),             QString("%1").arg(bmscr->objmax))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/objlimit"),           QString("%1").arg(bmscr->objlimit))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/savepos"),            QString(""))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/master_hspwnd"),      QString("0x%1").arg((uint)bmscr->master_hspwnd, 8, 16, QLatin1Char('0')))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/palcolor"),           QString("%1").arg(bmscr->palcolor))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/textstyle"),          QString("%1").arg(bmscr->textstyle))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/framesx"),            QString("%1").arg(bmscr->framesx))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/framesy"),            QString("%1").arg(bmscr->framesy))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/imgbtn"),             QString("%1").arg(bmscr->imgbtn))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/btn_x1"),             QString("%1").arg(bmscr->btn_x1))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/btn_y1"),             QString("%1").arg(bmscr->btn_y1))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/btn_x2"),             QString("%1").arg(bmscr->btn_x2))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/btn_y2"),             QString("%1").arg(bmscr->btn_y2))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/btn_x3"),             QString("%1").arg(bmscr->btn_x3))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/btn_y3"),             QString("%1").arg(bmscr->btn_y3))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/divx"),               QString("%1").arg(bmscr->divx))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/divy"),               QString("%1").arg(bmscr->divy))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/divsx"),              QString("%1").arg(bmscr->divsx))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/divsy"),              QString("%1").arg(bmscr->divsy))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/celofsx"),            QString("%1").arg(bmscr->celofsx))
			<< qMakePair(codec->toUnicode("�V�X�e�����/bmscr/celofsy"),            QString("%1").arg(bmscr->celofsy))
			;
	}

	debugInfo
		<< qMakePair(codec->toUnicode("�V�X�e���ϐ�/hspstat"), QString("%1").arg(hspctx->hspstat))
		<< qMakePair(codec->toUnicode("�V�X�e���ϐ�/hspver"),  QString("%1").arg(hspver))
		<< qMakePair(codec->toUnicode("�V�X�e���ϐ�/stat"),    QString("%1").arg(hspctx->stat))
		<< qMakePair(codec->toUnicode("�V�X�e���ϐ�/cnt"),     QString("%1").arg(hspctx->mem_loop[hspctx->looplev].cnt))
		<< qMakePair(codec->toUnicode("�V�X�e���ϐ�/err"),     QString("%1").arg(hspctx->err))
		<< qMakePair(codec->toUnicode("�V�X�e���ϐ�/strsize"), QString("%1").arg(hspctx->strsize))
		<< qMakePair(codec->toUnicode("�V�X�e���ϐ�/looplev"), QString("%1").arg(hspctx->looplev))
		<< qMakePair(codec->toUnicode("�V�X�e���ϐ�/sublev"),  QString("%1").arg(hspctx->sublev))
		<< qMakePair(codec->toUnicode("�V�X�e���ϐ�/iparam"),  QString("%1").arg(hspctx->iparam))
		<< qMakePair(codec->toUnicode("�V�X�e���ϐ�/wparam"),  QString("%1").arg(hspctx->wparam))
		<< qMakePair(codec->toUnicode("�V�X�e���ϐ�/lparam"),  QString("%1").arg(hspctx->lparam))
		<< qMakePair(codec->toUnicode("�V�X�e���ϐ�/refstr"),  hspctx->refstr ? QString("\"%1\"").arg(codec->toUnicode(hspctx->refstr)) : QString(""))
		<< qMakePair(codec->toUnicode("�V�X�e���ϐ�/refdval"), QString("%1").arg(hspctx->refdval, 0, 'g'))
		;
	
	// IDE���ɓ]��
	QByteArray  data;
	QDataStream out(&data, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_4);
	out << debugInfo;
	IpcSend(*m_socket, CMD_UPDATE_DEBUG_INFO, data);
}

void CDbgMain::updateVarInfo()
{
	//		�ϐ����擾
	//		option
	//			bit0 : sort ( �󂯑��ŏ��� )
	//			bit1 : module
	//			bit2 : array
	//			bit3 : dump

	QVector<VARIABLE_INFO_TYPE> varInfo;

	for(int i = 0, num = m_varNames.size();
		i < num; i++)
	{
		varInfo.push_back(VARIABLE_INFO_TYPE());
		VARIABLE_INFO_TYPE & info = varInfo.back();
		// �ϐ����擾
		int indexOf[4] = {0,-1,-1,-1};
		GetVariableInfo(i, indexOf, info);
	}

	QByteArray data;
	QDataStream out(&data, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_4);
	out << varInfo;
	IpcSend(*m_socket, CMD_PUT_VAR_DIGEST, data);
}

bool CDbgMain::GetVariableInfo(const QString& varName, int indexOf[], VARIABLE_INFO_TYPE& varInfo)
{
	int indexOfVar = -1;

	for(int i = 0, num = m_varNames.size();
		i < num; i++)
	{
		if( varName == m_varNames[i] ) {
			indexOfVar = i;
			break;
		}
	}
	if( indexOfVar < 0 ) {
		return false;
	}

	return GetVariableInfo(indexOfVar, indexOf, varInfo);
}

bool CDbgMain::GetVariableInfo(int indexOfVariable, int indexOf[], VARIABLE_INFO_TYPE& varInfo)
{
	QTextCodec* codec = QTextCodec::codecForLocale();

	HSPCTX* hspctx = (HSPCTX*)m_dbg->hspctx;

	PVal* pval = &hspctx->mem_var[indexOfVariable];

	// �ϐ���
	varInfo.name = m_varNames[indexOfVariable];

	// �ϐ����
	switch(pval->flag) {
	case HSPVAR_FLAG_NONE:		varInfo.typeName = "?";			break;
	case HSPVAR_FLAG_LABEL:		varInfo.typeName = "label";		break;
	case HSPVAR_FLAG_STR:		varInfo.typeName = "string";	break;
	case HSPVAR_FLAG_DOUBLE:	varInfo.typeName = "double";	break;
	case HSPVAR_FLAG_INT:		varInfo.typeName = "int";		break;
	case HSPVAR_FLAG_STRUCT:	varInfo.typeName = "struct";	break;
	case HSPVAR_FLAG_COMSTRUCT:	varInfo.typeName = "COM";		break;
	default:
		if( HSPVAR_FLAG_USERDEF <= pval->flag ) {
			varInfo.typeName = QString("userdef-%1").arg(pval->flag - HSPVAR_FLAG_USERDEF + 1);
		}
	}

	for(int i = 0; i < 4; i++)
	{
		varInfo.index[i] = indexOf[i];
		varInfo.length[i]= pval->len[i + 1];
	}

	// �z��͈̓`�F�b�N
	int offset = pval->offset; // �I�t�Z�b�g��ۑ�
	for(int i = 0; i < 4 && 0 <= indexOf[i]; i++)
	{
		if( pval->len[i+1] <= indexOf[i] )
		{
			varInfo.description = "out of range";
			pval->offset = offset; // �I�t�Z�b�g�𕜋A
			return true;
		}
		// ���f�[�^�܂ł̃I�t�Z�b�g���v�Z
		if( 0 < i ) {
			pval->offset *= pval->len[i];
		}
		pval->offset += indexOf[i];
	}

	HspVarProc* varproc = hspctx->exinfo.HspFunc_getproc(pval->flag);
	PDAT* pd  = varproc->GetPtr(pval);
	int   len = varproc->GetSize(pd);
	pval->offset = offset; // �I�t�Z�b�g�𕜋A

	// �ϐ��T�v
	switch(pval->flag) {
	case HSPVAR_FLAG_LABEL:
		break;
	case HSPVAR_FLAG_STR:
		varInfo.description = QString("\"%1\"").arg(codec->toUnicode((char*)pd, len));
		break;
	case HSPVAR_FLAG_DOUBLE:
		varInfo.description = QString("%1").arg(*(double*)pd);
		break;
	case HSPVAR_FLAG_INT:
		varInfo.description = QString("%1").arg(*(int*)pd);
		break;
	case HSPVAR_FLAG_STRUCT:
		break;
	case HSPVAR_FLAG_COMSTRUCT:
		break;
	default:
		if( HSPVAR_FLAG_USERDEF <= pval->flag ) {
		}
		break;
	case HSPVAR_FLAG_NONE:
		break;
	}

#ifdef _DEBUG
	varInfo.description +=
			QString("|%1,%2,%3,[%5,%6,%7,%8,%9]")
				.arg(pval->flag)
				.arg(pval->mode)
				.arg(pval->size)
				.arg(pval->len[0])
				.arg(pval->len[1])
				.arg(pval->len[2])
				.arg(pval->len[3])
				.arg(pval->len[4]);
#endif

	return true;
}

void CDbgMain::ReqestVariableInfo(const QString& varName, int indexOf[])
{
	VARIABLE_INFO_TYPE info;

	if( !GetVariableInfo(varName, indexOf, info) )
	{
	}
	else
	{
		QByteArray data;
		QDataStream out(&data, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_4_4);
		out << info;
		IpcSend(*m_socket, CMD_RES_VAR_INFO, data);
	}

}

void CDbgMain::connected()
{

}

void CDbgMain::disconnected()
{
OutputDebugStringA("CDbgMain::disconnected\n");
	QMutexLocker lck(&m_lock);
	m_quit = true;
	HSPCTX* hspctx = (HSPCTX*)m_dbg->hspctx;
	BMSCR*  bmscr  = (BMSCR*)(*hspctx->exinfo2->HspFunc_getbmscr)(0);
	if( bmscr ) {
		::PostMessage((HWND)bmscr->hwnd, WM_QUIT, 0, 0);
	} else { // �E�C���h�E�Ȃ�
		TerminateProcess(GetCurrentProcess(), 0);
	}
	::SetEvent(m_waitDisconnect);
}

void CDbgMain::disconnectLater()
{
	disconnect(m_socket, SIGNAL(readyRead()), this, SLOT(recvCommand()));
	m_socket->disconnectFromServer();
}

void CDbgMain::recvCommand()
{
	CMD_ID     cmd_id;
	QByteArray param;

	while( IpcRecv(*m_socket, cmd_id, param) )
	{
		switch(cmd_id)
		{
		case CMD_SET_BREAK_POINT: { // �u���[�N�|�C���g��ݒ�
			QMutexLocker lck(&m_lock);
			QDataStream in(param);
			in.setVersion(QDataStream::Qt_4_4);
			in >> m_lookup >> m_bp;
			// ������������ʒm
			::SetEvent(m_waitThread);
			break; }
		case CMD_SUSPEND_DEBUG: { // �f�o�b�O���~
			debugSuspend();
			break; }
		case CMD_RESUME_DEBUG: { // �f�o�b�O���ĊJ
			QMutexLocker lck(&m_lock);
			m_breaked = false;
			m_resumed = true;
			break; }
		case CMD_STOP_DEBUG: { // �f�o�b�O�𒆎~
			disconnectFromDebugger();
			break; }
		case CMD_STEP_IN_DEBUG: { // �X�e�b�vIN
			QMutexLocker lck(&m_lock);
			m_stepIn = true;
			break; }
		case CMD_REQ_VAR_INFO: { // �ϐ�����v��
			QMutexLocker lck(&m_lock);
			QDataStream in(param);
			in.setVersion(QDataStream::Qt_4_4);
			QString varName;
			int indexOf[4];
			in	>> varName
				>> indexOf[0] >> indexOf[1]
				>> indexOf[2] >> indexOf[3];
			ReqestVariableInfo(varName, indexOf);
			break; }
		default: {
			qDebug() <<__FUNCTION__<< (void*)m_socket<< cmd_id;
			}
		}
#ifdef _DEBUG
	char tmp[256];sprintf(tmp,"%s %d\n",__FUNCTION__,cmd_id);OutputDebugStringA(tmp);
#endif
	}
}
