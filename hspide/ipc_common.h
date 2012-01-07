#include <QDebug>
#include <QDataStream>
#include <QLocalSocket>
#include <QByteArray>
#include <QString>

#pragma once

//-------------------------------------------------------------------
// VARIABLE_INFO_TYPE型、専用operator定義

typedef struct {
	QString name;
	QString typeName;
	QString description;
} VARIABLE_INFO_TYPE;

QDebug&      operator<<(QDebug&      debug,  const VARIABLE_INFO_TYPE& info);
QDataStream& operator<<(QDataStream& stream, const VARIABLE_INFO_TYPE& info);
QDataStream& operator>>(QDataStream& stream,       VARIABLE_INFO_TYPE& info);

//-------------------------------------------------------------------
// CMD_ID型、専用operator定義

typedef enum {
	CMD_CONNECT = 0,		// デバッガに接続
	CMD_PUT_LOG,			// ログを出力
	CMD_SET_BREAK_POINT,	// ブレークポイントを設定
	CMD_STOP_RUNNING,		// 実行の停止
	CMD_SUSPEND_DEBUG,		// デバッグを停止
	CMD_RESUME_DEBUG,		// デバッグを再開
	CMD_STOP_DEBUG,			// デバッグを中止
	CMD_UPDATE_DEBUG_INFO,	// デバッグ情報を更新
	CMD_PUT_VAR_DIGEST,		// 変数情報の概要を通知
	CMD_REQ_VAR_INFO,		// 変数情報を要求
	CMD_RES_VAR_INFO,		// 変数情報を返答
} CMD_ID;

QDebug&      operator<<(QDebug&      debug,  const CMD_ID& v);
QDataStream& operator<<(QDataStream& stream, const CMD_ID& v);
QDataStream& operator>>(QDataStream& stream,       CMD_ID& v);

//-------------------------------------------------------------------

bool IpcSend(QLocalSocket& socket, CMD_ID  cmd, const QByteArray& param = QByteArray());
bool IpcRecv(QLocalSocket& socket, CMD_ID& cmd,       QByteArray& param);
