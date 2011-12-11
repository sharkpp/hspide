#include "dbgmain.h"
#include "../hspide/debuggercommand.hpp"

// dummy argument for QCoreApplication
static int argc_ = 1;
static char *argv_[] = {""};

CDbgMain::CDbgMain()
	: QCoreApplication(argc_, argv_)
	, m_socket(new QLocalSocket(this))
	, m_id(_strtoui64(getenv("hspide#attach"), NULL, 16))
{
	m_socket->connectToServer("test@hspide");
}

CDbgMain::~CDbgMain()
{
	m_socket->disconnectFromServer();
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
