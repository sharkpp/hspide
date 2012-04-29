#include <QDebug>
#include <QTextCodec>
#include <QDataStream>
#include "workspaceitem.h"
#include "compiler.h"
#include "debugger.h"

CDebugger::CDebugger(QObject *parent, QLocalSocket* socket)
	: QObject(parent)
	, m_clientConnection(socket)
	, m_targetItem(NULL)
{
	connect(m_clientConnection, SIGNAL(readyRead()), this, SLOT(recvCommand()));
	connect(m_clientConnection, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}

// �ݒ�X�V
void CDebugger::setConfiguration(const Configuration& info)
{
	connect(&info, SIGNAL(updateConfiguration(const Configuration&)),
	        this,  SLOT(updateConfiguration(const Configuration&)));
	updateConfiguration(info);
}

void CDebugger::updateConfiguration(const Configuration& info)
{
}

void CDebugger::recvCommand()
{
	CMD_ID cmd_id;
	QByteArray param;

	while( IpcRecv(*m_clientConnection, cmd_id, param) )
	{
		switch(cmd_id)
		{
		case CMD_CONNECT: { // �f�o�b�K�ɐڑ�
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << cmd_id;
			CCompiler* compiler = qobject_cast<CCompiler*>(parent());
			if( compiler )
			{
				// id�擾
				QDataStream in(param);
				in.setVersion(QDataStream::Qt_4_4);
				qint64 id;
				in >> id;
				// �֘A�t����ꂽ�A�C�e�����擾
				m_targetItem = compiler->getTargetItem(id);
				// �u���[�N�|�C���g���X�V
				updateBreakpoint();
			}
			break; }
		case CMD_PUT_LOG: { // ���O���o��
			QTextCodec* codec = QTextCodec::codecForLocale();
			QString s = codec->toUnicode(param);
#ifdef _DEBUG
			if( "?{" != s.left(2) ) // �Ƃ肠���������p�̃��O�̓t�B���^
#endif
				emit putLog(s);
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << cmd_id << s;
			break; }
		case CMD_STOP_RUNNING: { // ���s�̒�~
			QDataStream in(param);
			in.setVersion(QDataStream::Qt_4_4);
			QUuid uuid;
			int   lineNo;
			in >> uuid >> lineNo;
			emit stopAtBreakPoint(uuid, lineNo);
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << cmd_id << uuid << lineNo;
			break; }
		case CMD_UPDATE_DEBUG_INFO: { // �f�o�b�O�����X�V
			QDataStream in(param);
			in.setVersion(QDataStream::Qt_4_4);
			in >> m_debugInfo;
			emit updateDebugInfo(m_debugInfo);
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << cmd_id << m_debugInfo;
			break; }
		case CMD_PUT_VAR_DIGEST: { // �ϐ����̊T�v��ʒm
			QDataStream in(param);
			in.setVersion(QDataStream::Qt_4_4);
			QVector<VARIABLE_INFO_TYPE> varInfo;
			in >> varInfo;
			emit updateVarInfo(varInfo);
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << cmd_id << varInfo;
			break; }
		case CMD_RES_VAR_INFO: { // �ϐ�����ԓ�
			QDataStream in(param);
			in.setVersion(QDataStream::Qt_4_4);
			QVector<VARIABLE_INFO_TYPE> varInfo(1);
			in >> varInfo.back();
			emit updateVarInfo(varInfo);
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << cmd_id << varInfo;
			break; }
		default:
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << cmd_id;
		}
	}
}

// �f�o�b�O�𒆒f
void CDebugger::suspendDebugging()
{
	IpcSend(*m_clientConnection, CMD_SUSPEND_DEBUG);
}

// �f�o�b�O���ĊJ
void CDebugger::resumeDebugging()
{
	IpcSend(*m_clientConnection, CMD_RESUME_DEBUG);
}

// �f�o�b�O���~
void CDebugger::stopDebugging()
{
	IpcSend(*m_clientConnection, CMD_STOP_DEBUG);
}

// �ϐ�����v��
void CDebugger::reqVariableInfo(const QString& varName, int info[])
{
	QByteArray  data;
	QDataStream out(&data, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_4);
	out << varName
		<< info[0] << info[1]
		<< info[2] << info[3];
	IpcSend(*m_clientConnection, CMD_REQ_VAR_INFO, data);
}

// �u���[�N�|�C���g���X�V
void CDebugger::updateBreakpoint()
{
	CBreakPointInfo bp;
	CUuidLookupInfo lookup;

	if( m_targetItem &&
		m_targetItem->getBreakPoints(bp, lookup) )
	{
		// �u���[�N�|�C���g��DLL���֑��M
		QByteArray  data;
		QDataStream out(&data, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_4_4);
		out << lookup << bp;
		foreach(const QUuid &key, bp.keys()) {
			foreach(int lineNo, bp[key]) {
				qDebug() << "bp" << key << lineNo;
			}
		}
		IpcSend(*m_clientConnection, CMD_SET_BREAK_POINT, data);
	}
}
