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
	// 受信通知などのシグナルと接続
	connect(m_socket, SIGNAL(connected()),  this, SLOT(connected()));
	connect(m_socket, SIGNAL(readyRead()),  this, SLOT(recvCommand()));
	connect(m_socket, SIGNAL(disconnect()), this, SLOT(deleteLater()));

	// デバッガサーバーに接続
	m_socket->connectToServer("test@hspide");

	// 接続通知送信
	connectToDebugger();
}

CDbgMain::~CDbgMain()
{
	m_socket->disconnectFromServer();
}

void CDbgMain::create()
{
	// 通知イベント初期化
	m_waitThread = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	// スレッド開始
	m_handle
		= (HANDLE)::_beginthreadex(NULL, 0, CDbgMain::runStatic,
		                           (void*)&g_app, 0, NULL);
	// 初期化完了まで待機
	::WaitForSingleObject(m_waitThread, INFINITE);
}

void CDbgMain::destroy()
{
	// 終了要求
	g_app->quit();
	// 終了待機
	::WaitForSingleObject(m_waitThread, INFINITE);
	// リソース解放
	::CloseHandle(m_waitThread);
	::CloseHandle(m_handle);
}

unsigned CDbgMain::runStatic(void* p)
{
	CDbgMain* app
		= new CDbgMain();
	*static_cast<CDbgMain**>(p) = app;

	// 初期化完了を通知
	::SetEvent(m_waitThread);

	app->exec();

	delete app;

	// 終了完了を通知
	::SetEvent(m_waitThread);

	return 0;
}

void CDbgMain::connectToDebugger()
{
	CDebuggerCommand cmd;
	cmd.write(m_id, 0x00, NULL, 0);
	m_socket->write(QByteArray((char*)cmd.data(), cmd.size()));
}

void CDbgMain::putLog(const char *text, int len)
{
	CDebuggerCommand cmd;
	cmd.write(m_id, 0x01, text, len);
	m_socket->write(QByteArray((char*)cmd.data(), cmd.size()));
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
		default: {
			CDebuggerCommand cmd;
			cmd.write(m_id, 0x02, "", 0);
			m_socket->write(QByteArray((char*)cmd.data(), cmd.size()));
			qDebug() <<"CDbgMain::recvCommand"<< (void*)m_socket<< id << cmd_id;
			}
		}
	}
}
