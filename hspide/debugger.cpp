#include <QDebug>
#include <QTextCodec>
#include "debugger.h"

CDebugger::CDebugger(QObject *parent, QLocalSocket* socket)
	: QObject(parent)
	, m_clientConnection(socket)
{
	connect(m_clientConnection, SIGNAL(readyRead()), this, SLOT(recvCommand()));
}

void CDebugger::parseCommand()
{
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
		case 0x00: {
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << id << cmd_id;
			break; }
		case 0x01: {
			QTextCodec* codec = QTextCodec::codecForLocale();
			QString s = codec->toUnicode(QByteArray((const char*)ptr.data(), ptr.size()));
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << id << cmd_id << length << s;
			break; }
		default:
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection<< id << cmd_id;
		}
CDebuggerCommand cmd;
cmd.write(id, 0x02, "aaa", 3);
m_clientConnection->write(QByteArray((char*)cmd.data(), cmd.size()));
	}
}

void CDebugger::recvCommand()
{
CDebuggerCommand cmd;
cmd.write(0, 0x02, "aaa", 3);
(qobject_cast<QLocalSocket*>(sender()))->write(QByteArray((char*)cmd.data(), cmd.size()));

	QByteArray data = m_clientConnection->readAll();
	m_cmdQueue.push(data.data(), data.size());
	parseCommand();
}
