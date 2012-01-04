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
		case CDebuggerCommand::CMD_CONNECT: { // �f�o�b�K�ɐڑ�
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
		case CDebuggerCommand::CMD_PUT_LOG: { // ���O���o��
			QTextCodec* codec = QTextCodec::codecForLocale();
			QString s = codec->toUnicode(QByteArray((const char*)ptr.data(), ptr.size()));
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << id << cmd_id << length << s;
			break; }
		case CDebuggerCommand::CMD_STOP_RUNNING: { // ���s�̒�~
			QByteArray data((char*)ptr.data(), ptr.size());
			QDataStream in(data);
			in.setVersion(QDataStream::Qt_4_4);
			QUuid uuid;
			int lineNo;
			in >> uuid >> lineNo;
			emit stopAtBreakPoint(uuid, lineNo);
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << id << cmd_id << uuid << lineNo;
			break; }
		case CDebuggerCommand::CMD_UPDATE_DEBUG_INFO: { // �f�o�b�O�����X�V
			QByteArray data((char*)ptr.data(), ptr.size());
			QDataStream in(data);
			in.setVersion(QDataStream::Qt_4_4);
			in >> m_debugInfo;
			emit updateDebugInfo(m_debugInfo);
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << id << cmd_id << m_debugInfo;
			break; }
		case CDebuggerCommand::CMD_UPDATE_VAR_INFO: { // �ϐ��������X�V
			QByteArray data((char*)ptr.data(), ptr.size());
			QDataStream in(data);
			in.setVersion(QDataStream::Qt_4_4);
			QVector<VARIABLE_INFO_TYPE> varInfo;
			in >> varInfo;
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << id << cmd_id << varInfo;
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

// �f�o�b�O�𒆒f
void CDebugger::suspendDebugging()
{
	CDebuggerCommand cmd;
	cmd.write(0, CDebuggerCommand::CMD_SUSPEND_DEBUG);
	m_clientConnection->write(QByteArray((char*)cmd.data(), cmd.size()));
}

// �f�o�b�O���ĊJ
void CDebugger::resumeDebugging()
{
	CDebuggerCommand cmd;
	cmd.write(0, CDebuggerCommand::CMD_RESUME_DEBUG);
	m_clientConnection->write(QByteArray((char*)cmd.data(), cmd.size()));
}

// �f�o�b�O���~
void CDebugger::stopDebugging()
{
	CDebuggerCommand cmd;
	cmd.write(0, CDebuggerCommand::CMD_STOP_DEBUG);
	m_clientConnection->write(QByteArray((char*)cmd.data(), cmd.size()));
}
