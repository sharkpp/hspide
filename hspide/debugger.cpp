#include <QDebug>
#include <QTextCodec>
#include <QDataStream>
#include "compiler.h"
#include "debugger.h"

CDebugger::CDebugger(QObject *parent, QLocalSocket* socket)
	: QObject(parent)
	, m_clientConnection(socket)
{
	connect(m_clientConnection, SIGNAL(readyRead()), this, SLOT(recvCommand()));
	connect(m_clientConnection, SIGNAL(destroyed()), this, SLOT(deleteLater()));
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
		case CDebuggerCommand::CMD_CONNECT: {
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << id << cmd_id;
			CCompiler* compiler = qobject_cast<CCompiler*>(parent());
			if( compiler )
			{
				CBreakPointInfo bp;
				CUuidLookupInfo lookup;
				compiler->getBreakPoint(id, bp, lookup);
				QByteArray data;
				QDataStream out(&data, QIODevice::WriteOnly);
				out.setVersion(QDataStream::Qt_4_4);
				out << lookup << bp;
				foreach(const QUuid &key, bp.keys()) {
					foreach(int lineNo, bp[key]) {
						qDebug() << "bp" << key << lineNo;
					}
				}
				CDebuggerCommand cmd;
				cmd.write(id, CDebuggerCommand::CMD_SET_BREAK_POINT, data.data(), data.size());
				m_clientConnection->write(QByteArray((char*)cmd.data(), cmd.size()));
			}
			break; }
		case CDebuggerCommand::CMD_PUT_LOG: {
			QTextCodec* codec = QTextCodec::codecForLocale();
			QString s = codec->toUnicode(QByteArray((const char*)ptr.data(), ptr.size()));
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << id << cmd_id << length << s;
			break; }
		case CDebuggerCommand::CMD_STOP_RUNNING: {
			QByteArray data((char*)ptr.data(), ptr.size());
			QDataStream in(data);
			in.setVersion(QDataStream::Qt_4_4);
			QUuid uuid;
			int lineNum;
			in >> uuid >> lineNum;
			emit stopAtBreakPoint(uuid, lineNum);
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << id << cmd_id << uuid << lineNum;
			break; }
		default:
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection<< id << cmd_id;
		}
	}
}

void CDebugger::recvCommand()
{
	QByteArray data = m_clientConnection->readAll();
	m_cmdQueue.push(data.data(), data.size());
	parseCommand();
}

// デバッグを中断
void CDebugger::suspendDebugging()
{
	CDebuggerCommand cmd;
	cmd.write(0, CDebuggerCommand::CMD_SUSPEND_DEBUG);
	m_clientConnection->write(QByteArray((char*)cmd.data(), cmd.size()));
}

// デバッグを停止
void CDebugger::stopDebugging()
{
	CDebuggerCommand cmd;
	cmd.write(0, CDebuggerCommand::CMD_STOP_DEBUG);
	m_clientConnection->write(QByteArray((char*)cmd.data(), cmd.size()));
}
