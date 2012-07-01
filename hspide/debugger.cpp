#include <QDebug>
#include <QTextCodec>
#include <QDataStream>
#include "workspaceitem.h"
#include "compilerset.h"
#include "debugger.h"

CDebugger::CDebugger(QObject *parent, QLocalSocket* socket)
	: QObject(parent)
	, m_clientConnection(socket)
	, m_targetItem(NULL)
{
	connect(m_clientConnection, SIGNAL(readyRead()),    this, SLOT(recvCommand()));
	connect(m_clientConnection, SIGNAL(destroyed()),    this, SLOT(deleteLater()));

	// �ݒ�̕ύX�ʎj�̓o�^�Ɛݒ肩��̏���������
	connect(&theConf, SIGNAL(updateConfiguration(const Configuration&)),
	        this,  SLOT(updateConfiguration(const Configuration&)));
	updateConfiguration(theConf);
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
		case CMD_CONNECT:			onRecvConnect(param);			break;
		case CMD_PUT_LOG:			onRecvPutLog(param);			break;
		case CMD_STOP_RUNNING:		onRecvStopRunning(param);		break;
		case CMD_UPDATE_DEBUG_INFO:	onRecvUpdateDebugInfo(param);	break;
		case CMD_PUT_VAR_DIGEST:	onRecvPutVarDigest(param);		break;
		case CMD_RES_VAR_INFO:		onRecvResVarInfo(param);		break;
		default:
			qDebug() <<__FUNCTION__<< (void*)m_clientConnection << cmd_id;
		}
	}
}

void CDebugger::onRecvConnect(const QByteArray& param)
{
	qDebug() <<__FUNCTION__<< (void*)m_clientConnection << CMD_CONNECT;
	// �f�o�b�K�ɐڑ�
	if( CCompilerSet* compilers = qobject_cast<CCompilerSet*>(parent()) )
	{
		// id�擾
		QDataStream in(param);
		in.setVersion(QDataStream::Qt_4_4);
		QString id;
		in >> id;
		// �֘A�t����ꂽ�A�C�e�����擾
		m_targetItem = compilers->getTargetItem(id);
		// �u���[�N�|�C���g���X�V
		updateBreakpoint();
	}
}

void CDebugger::onRecvPutLog(const QByteArray& param)
{
	// ���O���o��
	QTextCodec* codec = QTextCodec::codecForLocale();
	QString s = codec->toUnicode(param);
#ifdef _DEBUG
	if( !s.contains("typeinfo_hook::") ) // �Ƃ肠���������p�̃��O�̓t�B���^
#endif
	emit putLog(s);
	qDebug() <<__FUNCTION__<< (void*)m_clientConnection << CMD_PUT_LOG << s;
}

void CDebugger::onRecvStopRunning(const QByteArray& param)
{
	// ���s�̒�~
	QDataStream in(param);
	in.setVersion(QDataStream::Qt_4_4);
	QUuid uuid;
	int   lineNo;
	in >> uuid >> lineNo;
	emit stopAtBreakPoint(uuid, lineNo);
	emit stopCode(uuid, lineNo);
	qDebug() <<__FUNCTION__<< (void*)m_clientConnection << CMD_STOP_RUNNING << uuid << lineNo;
}

void CDebugger::onRecvUpdateDebugInfo(const QByteArray& param)
{
	// �f�o�b�O�����X�V
	QDataStream in(param);
	in.setVersion(QDataStream::Qt_4_4);
	in >> m_debugInfo;
	emit updateDebugInfo(m_debugInfo);
	qDebug() <<__FUNCTION__<< (void*)m_clientConnection << CMD_UPDATE_DEBUG_INFO << m_debugInfo;
}

void CDebugger::onRecvPutVarDigest(const QByteArray& param)
{
	// �ϐ����̊T�v��ʒm
	QDataStream in(param);
	in.setVersion(QDataStream::Qt_4_4);
	QVector<VARIABLE_INFO_TYPE> varInfo;
	in >> varInfo;
	emit updateVarInfo(varInfo);
	qDebug() <<__FUNCTION__<< (void*)m_clientConnection << CMD_PUT_VAR_DIGEST << varInfo;
}

void CDebugger::onRecvResVarInfo(const QByteArray& param)
{
	// �ϐ�����ԓ�
	QDataStream in(param);
	in.setVersion(QDataStream::Qt_4_4);
	QVector<VARIABLE_INFO_TYPE> varInfo(1);
	in >> varInfo.back();
	emit updateVarInfo(varInfo);
	qDebug() <<__FUNCTION__<< (void*)m_clientConnection << CMD_RES_VAR_INFO << varInfo;
}

// �f�o�b�O�𒆒f
void CDebugger::suspendDebugging()
{
	qDebug() <<__FUNCTION__<< CMD_SUSPEND_DEBUG;
	IpcSend(*m_clientConnection, CMD_SUSPEND_DEBUG);
}

// �f�o�b�O���ĊJ
void CDebugger::resumeDebugging()
{
	qDebug() <<__FUNCTION__<< CMD_RESUME_DEBUG;
	IpcSend(*m_clientConnection, CMD_RESUME_DEBUG);
}

// �f�o�b�O���~
void CDebugger::stopDebugging()
{
	qDebug() <<__FUNCTION__<< CMD_STOP_DEBUG;
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
	qDebug() <<__FUNCTION__<< CMD_REQ_VAR_INFO<< data;
	IpcSend(*m_clientConnection, CMD_REQ_VAR_INFO, data);
}

// �u���[�N�|�C���g���X�V
void CDebugger::updateBreakpoint()
{
	// ���C���̃t�@�C��������̏ꍇ�� ???? �����UUID��������悤�ɂ���
	FileManager theFile_ = theFile;
	if( m_targetItem->isUntitled() ) {
		theFile_.assign("????", m_targetItem->uuid());
	}
	// �u���[�N�|�C���g��DLL���֑��M
	QByteArray  data;
	QDataStream out(&data, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_4);
	out << theFile_ << theBreakPoint;
	qDebug() <<__FUNCTION__<< CMD_SET_BREAK_POINT << theFile << theBreakPoint;
	IpcSend(*m_clientConnection, CMD_SET_BREAK_POINT, data);
}
