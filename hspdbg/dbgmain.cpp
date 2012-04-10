#include <QTextCodec>
#include <process.h>
#include "dbgmain.h"
#include <spplib/apihook.h>
#include "hspsdk/hsp3debug.h"
#include "hspsdk/hsp3struct.h"
#include "hspsdk/hspwnd.h"

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

//--------------------------------------------------------------------
// typeinfo_hook�N���X

CDbgMain::typeinfo_hook::typeinfo_hook()
	: m_cmdfunc_thunk(this, cmdfunc_thunk) // C4355
	, m_reffunc_thunk(this, reffunc_thunk) // C4355
	, m_termfunc_thunk(this, termfunc_thunk) // C4355
	, m_msgfunc_thunk(this, msgfunc_thunk) // C4355
	, m_eventfunc_thunk(this, eventfunc_thunk) // C4355
	, m_typeinfo(NULL)
{
}

void CDbgMain::typeinfo_hook::install_hook(HSP3TYPEINFO* typeinfo)
{
	if( !m_typeinfo ) {
		m_typeinfo    = typeinfo;
		m_typeinfo_old= *typeinfo;
		// cmdfunc�̃t�b�N
		if( typeinfo->cmdfunc ) {
			typeinfo->cmdfunc = (int(*)(int))m_cmdfunc_thunk.get_code();
		}
		if( typeinfo->reffunc ) {
			typeinfo->reffunc = (void*(*)(int*,int))m_reffunc_thunk.get_code();
		}
		if( typeinfo->termfunc ) {
			typeinfo->termfunc = (int(*)(int))m_termfunc_thunk.get_code();
		}
		if( typeinfo->msgfunc ) {
			typeinfo->msgfunc = (int(*)(int,int,int))m_msgfunc_thunk.get_code();
		}
		if( typeinfo->eventfunc ) {
			typeinfo->eventfunc = (int(*)(int,int,int,void*))m_eventfunc_thunk.get_code();
		}
	}
}

int CALLBACK CDbgMain::typeinfo_hook::cmdfunc(int cmd)
{
	HSP3DEBUG* dbg = g_app->debugInfo();
	dbg->dbg_curinf();
	char tmp[256];
	sprintf(tmp,"%p %s(%d)/%d>>%s(%2d)",this,__FUNCTION__,cmd,m_typeinfo->type,dbg->fname?dbg->fname:"???",dbg->line);
	g_app->putLog(tmp, strlen(tmp));

	return m_typeinfo_old.cmdfunc(cmd);
}

void* CALLBACK CDbgMain::typeinfo_hook::reffunc(int *type_res, int arg)
{
	HSP3DEBUG* dbg = g_app->debugInfo();
	dbg->dbg_curinf();
	char tmp[256];
	sprintf(tmp,"%p %s(%p,%d)/%d>>%s(%2d)",this,__FUNCTION__,type_res,arg,m_typeinfo->type,dbg->fname?dbg->fname:"???",dbg->line);
	g_app->putLog(tmp, strlen(tmp));

	return m_typeinfo_old.reffunc(type_res, arg);
}

int CALLBACK CDbgMain::typeinfo_hook::termfunc(int option)
{
	HSP3DEBUG* dbg = g_app->debugInfo();
	dbg->dbg_curinf();
	char tmp[256];
	sprintf(tmp,"%p %s(%d)/%d>>%s(%2d)",this,__FUNCTION__,option,m_typeinfo->type,dbg->fname?dbg->fname:"???",dbg->line);
	g_app->putLog(tmp, strlen(tmp));

	return m_typeinfo_old.termfunc(option);
}

int CALLBACK CDbgMain::typeinfo_hook::msgfunc(int prm1,int prm2,int prm3)
{
	HSP3DEBUG* dbg = g_app->debugInfo();
	dbg->dbg_curinf();
	char tmp[256];
	sprintf(tmp,"%p %s(%d,%d,%d)/%d>>%s(%2d)",this,__FUNCTION__,prm1,prm2,prm3,m_typeinfo->type,dbg->fname?dbg->fname:"???",dbg->line);
	g_app->putLog(tmp, strlen(tmp));

	return m_typeinfo_old.msgfunc(prm1,prm2,prm3);
}

int CALLBACK CDbgMain::typeinfo_hook::eventfunc(int event, int prm1, int prm2, void *prm3)
{
	HSP3DEBUG* dbg = g_app->debugInfo();
	dbg->dbg_curinf();
	char tmp[256];
	sprintf(tmp,"%p %s(%d,%d,%d,%p)/%d>>%s(%2d)",this,__FUNCTION__,event,prm1,prm2,prm3,m_typeinfo->type,dbg->fname?dbg->fname:"???",dbg->line);
	g_app->putLog(tmp, strlen(tmp));

	return m_typeinfo_old.eventfunc(event,prm1,prm2,prm3);
}

char * CALLBACK CDbgMain::sbAlloc(int size)
{
	char *p = m_sbAlloc(size);

	HSP3DEBUG* dbg = g_app->debugInfo();
	char tmp[256];
	sprintf(tmp,"%p %s(%d) -> %p",this,__FUNCTION__,size,p);
	g_app->putLog(tmp, strlen(tmp));

	return p;
}

char * CALLBACK CDbgMain::sbExpand(char *ptr, int size)
{
	char *p = m_sbExpand(ptr,size);

	HSP3DEBUG* dbg = g_app->debugInfo();
	char tmp[256];
	sprintf(tmp,"%p %s(%p,%d) -> %p",this,__FUNCTION__,ptr,size,p);
	g_app->putLog(tmp, strlen(tmp));

	return p;
}

//--------------------------------------------------------------------
// CDbgMain�N���X

CDbgMain::CDbgMain()
	: QCoreApplication(argc_, argv_)
	, m_socket(new QLocalSocket(this))
	, m_id(_strtoui64(getenv("hspide#attach"), NULL, 16))
	, m_dbg(NULL)
	, m_breaked(false)
	, m_typeinfo_hook(NULL)
	, m_sbAlloc_thunk(this, sbAlloc_thunk)
	, m_sbExpand_thunk(this, sbExpand_thunk)
	, m_sbAlloc(NULL)
	, m_sbExpand(NULL)
{
	// ��M�ʒm�Ȃǂ̃V�O�i���Ɛڑ�
	connect(m_socket, SIGNAL(connected()),  this, SLOT(connected()));
	connect(m_socket, SIGNAL(readyRead()),  this, SLOT(recvCommand()));
	connect(m_socket, SIGNAL(disconnect()), this, SLOT(deleteLater()));

	// �f�o�b�K�T�[�o�[�ɐڑ�
	m_socket->connectToServer("test@hspide");

	// �ڑ��ʒm���M
	connectToDebugger();

	m_typeinfo_hook = new typeinfo_hook[1024];
}

CDbgMain::~CDbgMain()
{
	m_socket->disconnectFromServer();

	delete [] m_typeinfo_hook;
}

void CDbgMain::create()
{
	// �ʒm�C�x���g������
	m_waitThread = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	// �X���b�h�J�n
	m_handle
		= (HANDLE)::_beginthreadex(NULL, 0, CDbgMain::runStatic,
		                           (void*)&g_app, 0, NULL);
	// �����������܂őҋ@
	::WaitForSingleObject(m_waitThread, INFINITE);
}

void CDbgMain::destroy()
{
	// �I���v��
	g_app->quit();
	// �I���ҋ@
	::WaitForSingleObject(m_waitThread, INFINITE);
	// ���\�[�X���
	::CloseHandle(m_waitThread);
	::CloseHandle(m_handle);
}

unsigned CDbgMain::runStatic(void* p)
{
	CDbgMain* app
		= new CDbgMain();
	*static_cast<CDbgMain**>(p) = app;

	// ������������ʒm
//	::SetEvent(m_waitThread);

	app->exec();

	delete app;

	// �I��������ʒm
	::SetEvent(m_waitThread);

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

void CDbgMain::putLog(const char *text, int len)
{
	QByteArray  data(text, len);
	IpcSend(*m_socket, CMD_PUT_LOG, data);
}

bool CDbgMain::isBreak(const char* filename, int lineNo)
{
	QMutexLocker lck(&m_lock);

	// �f�o�b�K����̒�~�w��
	bool breaked = m_breaked;
	m_breaked = false;

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
		CUuidLookupInfo::iterator
			ite = m_lookup.find(fname);
		if( ite != m_lookup.end() )
		{
			uuid = *ite;
		}
	}
	if( !uuid.isNull() )
	{
		CBreakPointInfo::iterator
			ite = m_bp.find(uuid);
		if( ite != m_bp.end() &&
			ite->end() != ite->find(lineNo) )
		{
			breaked = true;
		}
	}

	if( breaked )
	{
		QByteArray  data;
		QDataStream out(&data, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_4_4);
		out << uuid << lineNo;
		IpcSend(*m_socket, CMD_STOP_RUNNING, data);
		return true;
	}

	return false;
}

// data seg ���� �������ǂݍ���
QString CDbgMain::loadString(HSPCTX* hspctx, int offset, bool allow_minus_idx)
{
	QTextCodec* codec = QTextCodec::codecForLocale();
	QString r;

	size_t         ds_len = hspctx->hsphed->max_ds;
	unsigned char* ds_ptr = (unsigned char*)hspctx->mem_mds;
	unsigned char* ds_last= ds_ptr + ds_len;

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
	unsigned char* ds_ptr = (unsigned char*)hspctx->mem_mds;
	unsigned char* ds_last= ds_ptr + ds_len;

	for(;di_ptr < di_last ; ) {
		switch( *di_ptr ) {
		case DINFO_TYPE_DATA_END:
			di_ptr++;
			break;
		case DINFO_TYPE_SOURCE_FILE_NAME:
			di_ptr += 4 + 2; // �t�@�C���� + �s�ԍ�
			break;
		case DINFO_TYPE_VARIABLE: {
			int offset = (int)((di_ptr[3] << 16) | (di_ptr[2] << 8) | di_ptr[1]);
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
	HSPCTX* hspctx = (HSPCTX*)top->hspctx;

	if( hspctx->exinfo.HspFunc_malloc ) {
		m_sbAlloc = (char*(*)(int))m_sbAlloc_thunk.injection_code(hspctx->exinfo.HspFunc_malloc, 7);
	}
	if( hspctx->exinfo.HspFunc_expand ) {
		m_sbExpand = (char*(*)(char*,int))m_sbExpand_thunk.injection_code(hspctx->exinfo.HspFunc_expand, 5);
	}

	for(HSP3TYPEINFO* ite = top; ite < last; ++ite)
	{
		// ��������
		m_typeinfo_hook[ite - top].install_hook(ite);
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

	char* ptr = m_dbg->get_value(DEBUGINFO_GENERAL);

	QVector<QPair<QString,QString> > debugInfo;

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

		debugInfo.append(qMakePair(key_str, value_str));
	}

	QByteArray  data;
	QDataStream out(&data, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_4);
	out << debugInfo;
	IpcSend(*m_socket, CMD_UPDATE_DEBUG_INFO, data);

	m_dbg->dbg_close(ptr);
}

void CDbgMain::updateVarInfo()
{
	QTextCodec* codec = QTextCodec::codecForLocale();

	//		�ϐ����擾
	//		option
	//			bit0 : sort ( �󂯑��ŏ��� )
	//			bit1 : module
	//			bit2 : array
	//			bit3 : dump
//	char* ptr = m_dbg->get_varinf(NULL, 2|4);

	HSPCTX* hspctx = (HSPCTX*)m_dbg->hspctx;

	QVector<VARIABLE_INFO_TYPE> varInfo;

	for(int i = 0, num = m_varNames.size();
		i < num; i++)
	{
		PVal* pval = &hspctx->mem_var[i];
		varInfo.push_back(VARIABLE_INFO_TYPE());
		VARIABLE_INFO_TYPE & info = varInfo.back();
		// �ϐ����擾
		int indexOf[4] = {0,-1,-1,-1};
		GetVariableInfo(i, indexOf, info);
	}

//	varInfo = m_varNames;

	QByteArray data;
	QDataStream out(&data, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_4);
	out << varInfo;
	IpcSend(*m_socket, CMD_PUT_VAR_DIGEST, data);

//	m_dbg->dbg_close(ptr);
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
			QMutexLocker lck(&m_lock);
			m_breaked = true;
			break; }
		case CMD_RESUME_DEBUG: { // �f�o�b�O���ĊJ
			QMutexLocker lck(&m_lock);
			m_breaked = false;
			break; }
		case CMD_STOP_DEBUG: { // �f�o�b�O�𒆎~
			HSPCTX* hspctx = (HSPCTX*)m_dbg->hspctx;
			BMSCR* bmscr = (BMSCR*)(*hspctx->exinfo2->HspFunc_getbmscr)(0);
			::PostMessage((HWND)bmscr->hwnd, WM_QUIT, 0, 0);
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
			qDebug() <<"CDbgMain::recvCommand"<< (void*)m_socket<< cmd_id;
			}
		}
	}
}
