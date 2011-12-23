#include <process.h>
#include "dbgmain.h"

extern CDbgMain* g_app;

// dummy argument for QCoreApplication
static int argc_ = 1;
static char *argv_[] = {""};

HANDLE	CDbgMain::m_handle = NULL;
HANDLE	CDbgMain::m_waitThread = NULL;

CDbgMain::CDbgMain()
	: QCoreApplication(argc_, argv_)
	, m_socket(new QLocalSocket(this))
	, m_id(_strtoui64(getenv("hspide#attach"), NULL, 16))
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
	CDebuggerCommand cmd;
	cmd.write(m_id, CDebuggerCommand::CMD_CONNECT, NULL, 0);
	m_socket->write(QByteArray((char*)cmd.data(), cmd.size()));
}

void CDbgMain::putLog(const char *text, int len)
{
	CDebuggerCommand cmd;
	cmd.write(m_id, CDebuggerCommand::CMD_PUT_LOG, text, len);
	m_socket->write(QByteArray((char*)cmd.data(), cmd.size()));
}

bool CDbgMain::isBreak(const char* filename, int lineNo)
{
	QMutexLocker lck(&m_lock);
	QString fname = filename && 0 != lstrcmpA(filename, "???") ? filename : "";
	CBreakPointInfo::iterator
		ite = m_bp.find(fname);
	if( ite != m_bp.end() &&
		ite->end() != ite->find(lineNo) )
	{
		CDebuggerCommand cmd;
		QByteArray data;
		QDataStream out(&data, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_4_4);
		out << fname << lineNo;
		cmd.write(m_id, CDebuggerCommand::CMD_STOP_RUNNING, data.data(), data.size());
		m_socket->write(QByteArray((char*)cmd.data(), cmd.size()));
		return true;
	}
	return false;
}

void CDbgMain::connected()
{
}

void CDbgMain::recvCommand()
{
	QByteArray data = m_socket->readAll();
	m_cmdQueue.push(data.data(), data.size());

	quint64 id;
	quint8  cmd_id;
	quint32 length;

	for(;;)
	{
		CDebuggerCommand::scoped_ptr ptr = m_cmdQueue.read(id, cmd_id, length);
		if( !ptr.valid() )
		{
			break;
		}

		switch(cmd_id)
		{
		case CDebuggerCommand::CMD_SET_BREAK_POINT: {
			QMutexLocker lck(&m_lock);
			QByteArray data((char*)ptr.data(), ptr.size());
			QDataStream in(data);
			in.setVersion(QDataStream::Qt_4_4);
			in >> m_bp;
			// ������������ʒm
			::SetEvent(m_waitThread);
			break; }
		default: {
//			CDebuggerCommand cmd;
//			cmd.write(m_id, 0x02, "", 0);
//			m_socket->write(QByteArray((char*)cmd.data(), cmd.size()));
			qDebug() <<"CDbgMain::recvCommand"<< (void*)m_socket<< id << cmd_id;
			}
		}
	}
}
