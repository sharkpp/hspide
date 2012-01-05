#include <QTextCodec>
#include <process.h>
#include "dbgmain.h"
#include "hspsdk/hsp3debug.h"
#include "hspsdk/hsp3struct.h"

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

CDbgMain::CDbgMain()
	: QCoreApplication(argc_, argv_)
	, m_socket(new QLocalSocket(this))
	, m_id(_strtoui64(getenv("hspide#attach"), NULL, 16))
	, m_breaked(false)
{
	// ��M�ʒm�Ȃǂ̃V�O�i���Ɛڑ�
	connect(m_socket, SIGNAL(connected()),  this, SLOT(connected()));
	connect(m_socket, SIGNAL(readyRead()),  this, SLOT(recvCommand()));
	connect(m_socket, SIGNAL(disconnect()), this, SLOT(deleteLater()));

	// �f�o�b�K�T�[�o�[�ɐڑ�
	m_socket->connectToServer("test@hspide");

	// �ڑ��ʒm���M
	connectToDebugger();
}

CDbgMain::~CDbgMain()
{
	m_socket->disconnectFromServer();
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

void CDbgMain::initializeVariableNames(HSP3DEBUG* dbg)
{
	HSPCTX* hspctx = (HSPCTX*)dbg->hspctx;

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
	initializeVariableNames(dbg);
}

void CDbgMain::updateInfo(HSP3DEBUG* dbg)
{
	// �f�o�b�O���X�V
	updateDebugInfo(dbg);

	// �ϐ����擾
	updateVarInfo(dbg);
}

void CDbgMain::updateDebugInfo(HSP3DEBUG* dbg)
{
	QTextCodec* codec = QTextCodec::codecForLocale();

	char* ptr = dbg->get_value(DEBUGINFO_GENERAL);

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

	dbg->dbg_close(ptr);
}

void CDbgMain::updateVarInfo(HSP3DEBUG* dbg)
{
	QTextCodec* codec = QTextCodec::codecForLocale();

	//		�ϐ����擾
	//		option
	//			bit0 : sort ( �󂯑��ŏ��� )
	//			bit1 : module
	//			bit2 : array
	//			bit3 : dump
//	char* ptr = dbg->get_varinf(NULL, 2|4);

	HSPCTX* hspctx = (HSPCTX*)dbg->hspctx;

	QVector<VARIABLE_INFO_TYPE> varInfo;

	//for(const char* p = ptr; *p;)
	//{
	//	const char* key = p;
	//	for(; *p && 0x0D != p[0] && 0x0A != p[1]; ++p);
	//	if( 0x0D != *p ) { break; }
	//	p += 2;

	//	QString key_str = codec->toUnicode(key, int(p - 2 - key));

	//	varInfo.append(key_str);
	//}

	for(int i = 0, num = m_varNames.size();
		i < num; i++)
	{
		PVal* pval = &hspctx->mem_var[i];
		varInfo.push_back(VARIABLE_INFO_TYPE());
		VARIABLE_INFO_TYPE & info = varInfo.back();
		info.name =
				QString("%1/%2,%3,%4,[%5,%6,%7,%8,%9]")
					.arg(m_varNames[i])
					.arg(pval->flag)
					.arg(pval->mode)
					.arg(pval->size)
					.arg(pval->len[0])
					.arg(pval->len[1])
					.arg(pval->len[2])
					.arg(pval->len[3])
					.arg(pval->len[4]);
		switch(pval->flag) {
		case HSPVAR_FLAG_NONE:		info.typeName = "?"; break;
		case HSPVAR_FLAG_LABEL:		info.typeName = "label"; break;
		case HSPVAR_FLAG_STR:		info.typeName = "string"; break;
		case HSPVAR_FLAG_DOUBLE:	info.typeName = "double"; break;
		case HSPVAR_FLAG_INT:		info.typeName = "int"; break;
		case HSPVAR_FLAG_STRUCT:	info.typeName = "struct"; break;
		case HSPVAR_FLAG_COMSTRUCT:	info.typeName = "com"; break;
		default:
			if( HSPVAR_FLAG_USERDEF <= pval->flag ) {
				info.typeName = QString("userdef-%1").arg(pval->flag - HSPVAR_FLAG_USERDEF + 1);
			}
		}
		
	}

//	varInfo = m_varNames;

	QByteArray data;
	QDataStream out(&data, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_4);
	out << varInfo;
	IpcSend(*m_socket, CMD_UPDATE_VAR_INFO, data);

//	dbg->dbg_close(ptr);
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
			break; }
		default: {
			qDebug() <<"CDbgMain::recvCommand"<< (void*)m_socket<< cmd_id;
			}
		}
	}
}
