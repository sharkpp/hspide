#include "ipc_common.h"

//-------------------------------------------------------------------
// VARIABLE_INFO_TYPE 用operator

QDebug& operator<<(QDebug& debug, const VARIABLE_INFO_TYPE& info)
{
	debug << "[" << info.name
	      << "/" << info.typeName
	      << "/" << info.description
	      << "/"
	      << "(" << info.index[0] << "," << info.index[1]
	      << "," << info.index[2] << "," << info.index[3]
		  << ")/"
	      << "(" << info.length[0] << "," << info.length[1]
	      << "," << info.length[2] << "," << info.length[3]
		  << ")"
		  << "]";
	return debug;
}

QDataStream& operator<<(QDataStream& stream, const VARIABLE_INFO_TYPE& info)
{
	stream	<< info.name
			<< info.typeName
			<< info.description
			<< info.index[0] << info.index[1]
			<< info.index[2] << info.index[3]
			<< info.length[0] << info.length[1]
			<< info.length[2] << info.length[3]
			;
	return stream;
}

QDataStream& operator>>(QDataStream& stream, VARIABLE_INFO_TYPE& info)
{
	stream	>> info.name
			>> info.typeName
			>> info.description
			>> info.index[0] >> info.index[1]
			>> info.index[2] >> info.index[3]
			>> info.length[0] >> info.length[1]
			>> info.length[2] >> info.length[3]
			;
	return stream;
}

//-------------------------------------------------------------------
// VARIABLE_INFO_TYPE 用operator

QDebug& operator<<(QDebug& debug, const CMD_ID& v)
{
	switch(v) {
 	case CMD_CONNECT:			debug << "CMD_CONNECT"; break;
	case CMD_PUT_LOG:			debug << "CMD_PUT_LOG"; break;
	case CMD_SET_BREAK_POINT:	debug << "CMD_SET_BREAK_POINT"; break;
	case CMD_STOP_RUNNING:		debug << "CMD_STOP_RUNNING"; break;
	case CMD_SUSPEND_DEBUG:		debug << "CMD_SUSPEND_DEBUG"; break;
	case CMD_RESUME_DEBUG:		debug << "CMD_RESUME_DEBUG"; break;
	case CMD_STOP_DEBUG:		debug << "CMD_STOP_DEBUG"; break;
	case CMD_STEP_IN_DEBUG:		debug << "CMD_STEP_IN_DEBUG"; break;
	case CMD_UPDATE_DEBUG_INFO:	debug << "CMD_UPDATE_DEBUG_INFO"; break;
	case CMD_PUT_VAR_DIGEST:	debug << "CMD_PUT_VAR_DIGEST"; break;
	case CMD_REQ_VAR_INFO:		debug << "CMD_REQ_VAR_INFO"; break;
	case CMD_RES_VAR_INFO:		debug << "CMD_RES_VAR_INFO"; break;
	}
	return debug;
}

QDataStream& operator<<(QDataStream& stream, const CMD_ID& v)
{
	stream << qint16(v);
	return stream;
}

QDataStream& operator>>(QDataStream& stream, CMD_ID& v)
{
	qint16 v_;
	stream >> v_;
	v = CMD_ID(v_);
	return stream;
}

//-------------------------------------------------------------------

bool IpcSend(QLocalSocket& socket, CMD_ID cmd, const QByteArray& param)
{
	QByteArray  data;
	QDataStream out(&data, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_4);
	out << cmd
		<< qint64(0)
		<< param;
	out.device()->seek(sizeof(qint16));
	out << qint64(data.size() - sizeof(qint16) - sizeof(qint64));
	socket.write(data);
	return true;
}

bool IpcRecv(QLocalSocket& socket, CMD_ID& cmd, QByteArray& param)
{
	qint64 param_len = 0;
	qint64 len = sizeof(qint16) + sizeof(param_len);
	qint64 bytesAvailable = socket.bytesAvailable();

	if( bytesAvailable < len )
	{
		return false;
	}

	// コマンドIDとパラメータを取得
	QDataStream in_(socket.peek(len));
	in_.setVersion(QDataStream::Qt_4_4);
	in_ >> cmd >> param_len;

	len += param_len;
	if( bytesAvailable < len )
	{
		return false;
	}

	QDataStream in(socket.read(len));
	in.setVersion(QDataStream::Qt_4_4);
	in >> cmd >> param_len >> param;

	return true;
}
