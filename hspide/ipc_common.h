#include <QDebug>
#include <QDataStream>
#include <QLocalSocket>
#include <QByteArray>
#include <QString>

#pragma once

//-------------------------------------------------------------------
// VARIABLE_INFO_TYPE�^�A��poperator��`

typedef struct {
	QString name;
	QString typeName;
	QString description;
} VARIABLE_INFO_TYPE;

QDebug&      operator<<(QDebug&      debug,  const VARIABLE_INFO_TYPE& info);
QDataStream& operator<<(QDataStream& stream, const VARIABLE_INFO_TYPE& info);
QDataStream& operator>>(QDataStream& stream,       VARIABLE_INFO_TYPE& info);

//-------------------------------------------------------------------
// CMD_ID�^�A��poperator��`

typedef enum {
	CMD_CONNECT = 0,		// �f�o�b�K�ɐڑ�
	CMD_PUT_LOG,			// ���O���o��
	CMD_SET_BREAK_POINT,	// �u���[�N�|�C���g��ݒ�
	CMD_STOP_RUNNING,		// ���s�̒�~
	CMD_SUSPEND_DEBUG,		// �f�o�b�O���~
	CMD_RESUME_DEBUG,		// �f�o�b�O���ĊJ
	CMD_STOP_DEBUG,			// �f�o�b�O�𒆎~
	CMD_UPDATE_DEBUG_INFO,	// �f�o�b�O�����X�V
	CMD_PUT_VAR_DIGEST,		// �ϐ����̊T�v��ʒm
	CMD_REQ_VAR_INFO,		// �ϐ�����v��
	CMD_RES_VAR_INFO,		// �ϐ�����ԓ�
} CMD_ID;

QDebug&      operator<<(QDebug&      debug,  const CMD_ID& v);
QDataStream& operator<<(QDataStream& stream, const CMD_ID& v);
QDataStream& operator>>(QDataStream& stream,       CMD_ID& v);

//-------------------------------------------------------------------

bool IpcSend(QLocalSocket& socket, CMD_ID  cmd, const QByteArray& param = QByteArray());
bool IpcRecv(QLocalSocket& socket, CMD_ID& cmd,       QByteArray& param);
