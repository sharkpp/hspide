#include "dbgmain.h"

// dummy argument for QCoreApplication
static int argc_ = 1;
static char *argv_[] = {""};

CDbgMain::CDbgMain()
	: QThread()
	, m_app(NULL)
	, m_socket(NULL)
	, m_id(_strtoui64(getenv("hspide#attach"), NULL, 16))
{
//	connect(m_thread, SIGNAL(finished()), m_thread, SLOT(quit()));
	start();
}

CDbgMain::~CDbgMain()
{
	m_socket->disconnectFromServer();
	m_app->quit();
}

void CDbgMain::run()
{
	QCoreApplication app(argc_, argv_);
	m_app = &app;

	m_socket = new QLocalSocket(&app);

	connect(m_socket, SIGNAL(connected()), this, SLOT(connected()));
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(recvCommand()));
	connect(m_socket, SIGNAL(disconnect()), this, SLOT(deleteLater()));

	m_socket->connectToServer("test@hspide");

	app.exec();
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
