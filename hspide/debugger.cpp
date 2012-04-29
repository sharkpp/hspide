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

// 設定更新
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
		case CMD_CONNECT: { // デバッガに接続
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << cmd_id;
			CCompiler* compiler = qobject_cast<CCompiler*>(parent());
			if( compiler )
			{
				// id取得
				QDataStream in(param);
				in.setVersion(QDataStream::Qt_4_4);
				qint64 id;
				in >> id;
				// 関連付けられたアイテムを取得
				m_targetItem = compiler->getTargetItem(id);
				// ブレークポイントを更新
				updateBreakpoint();
			}
			break; }
		case CMD_PUT_LOG: { // ログを出力
			QTextCodec* codec = QTextCodec::codecForLocale();
			QString s = codec->toUnicode(param);
#ifdef _DEBUG
			if( "?{" != s.left(2) ) // とりあえず内部用のログはフィルタ
#endif
				emit putLog(s);
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << cmd_id << s;
			break; }
		case CMD_STOP_RUNNING: { // 実行の停止
			QDataStream in(param);
			in.setVersion(QDataStream::Qt_4_4);
			QUuid uuid;
			int   lineNo;
			in >> uuid >> lineNo;
			emit stopAtBreakPoint(uuid, lineNo);
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << cmd_id << uuid << lineNo;
			break; }
		case CMD_UPDATE_DEBUG_INFO: { // デバッグ情報を更新
			QDataStream in(param);
			in.setVersion(QDataStream::Qt_4_4);
			in >> m_debugInfo;
			emit updateDebugInfo(m_debugInfo);
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << cmd_id << m_debugInfo;
			break; }
		case CMD_PUT_VAR_DIGEST: { // 変数情報の概要を通知
			QDataStream in(param);
			in.setVersion(QDataStream::Qt_4_4);
			QVector<VARIABLE_INFO_TYPE> varInfo;
			in >> varInfo;
			emit updateVarInfo(varInfo);
			qDebug() <<"CDebugger::recvCommand"<< (void*)m_clientConnection << cmd_id << varInfo;
			break; }
		case CMD_RES_VAR_INFO: { // 変数情報を返答
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

// デバッグを中断
void CDebugger::suspendDebugging()
{
	IpcSend(*m_clientConnection, CMD_SUSPEND_DEBUG);
}

// デバッグを再開
void CDebugger::resumeDebugging()
{
	IpcSend(*m_clientConnection, CMD_RESUME_DEBUG);
}

// デバッグを停止
void CDebugger::stopDebugging()
{
	IpcSend(*m_clientConnection, CMD_STOP_DEBUG);
}

// 変数情報を要求
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

// ブレークポイントを更新
void CDebugger::updateBreakpoint()
{
	CBreakPointInfo bp;
	CUuidLookupInfo lookup;

	if( m_targetItem &&
		m_targetItem->getBreakPoints(bp, lookup) )
	{
		// ブレークポイントをDLL側へ送信
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
