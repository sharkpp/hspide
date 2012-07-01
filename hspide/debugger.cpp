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

	// 設定の変更通史の登録と設定からの初期化処理
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
	// デバッガに接続
	if( CCompilerSet* compilers = qobject_cast<CCompilerSet*>(parent()) )
	{
		// id取得
		QDataStream in(param);
		in.setVersion(QDataStream::Qt_4_4);
		QString id;
		in >> id;
		// 関連付けられたアイテムを取得
		m_targetItem = compilers->getTargetItem(id);
		// ブレークポイントを更新
		updateBreakpoint();
	}
}

void CDebugger::onRecvPutLog(const QByteArray& param)
{
	// ログを出力
	QTextCodec* codec = QTextCodec::codecForLocale();
	QString s = codec->toUnicode(param);
#ifdef _DEBUG
	if( !s.contains("typeinfo_hook::") ) // とりあえず内部用のログはフィルタ
#endif
	emit putLog(s);
	qDebug() <<__FUNCTION__<< (void*)m_clientConnection << CMD_PUT_LOG << s;
}

void CDebugger::onRecvStopRunning(const QByteArray& param)
{
	// 実行の停止
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
	// デバッグ情報を更新
	QDataStream in(param);
	in.setVersion(QDataStream::Qt_4_4);
	in >> m_debugInfo;
	emit updateDebugInfo(m_debugInfo);
	qDebug() <<__FUNCTION__<< (void*)m_clientConnection << CMD_UPDATE_DEBUG_INFO << m_debugInfo;
}

void CDebugger::onRecvPutVarDigest(const QByteArray& param)
{
	// 変数情報の概要を通知
	QDataStream in(param);
	in.setVersion(QDataStream::Qt_4_4);
	QVector<VARIABLE_INFO_TYPE> varInfo;
	in >> varInfo;
	emit updateVarInfo(varInfo);
	qDebug() <<__FUNCTION__<< (void*)m_clientConnection << CMD_PUT_VAR_DIGEST << varInfo;
}

void CDebugger::onRecvResVarInfo(const QByteArray& param)
{
	// 変数情報を返答
	QDataStream in(param);
	in.setVersion(QDataStream::Qt_4_4);
	QVector<VARIABLE_INFO_TYPE> varInfo(1);
	in >> varInfo.back();
	emit updateVarInfo(varInfo);
	qDebug() <<__FUNCTION__<< (void*)m_clientConnection << CMD_RES_VAR_INFO << varInfo;
}

// デバッグを中断
void CDebugger::suspendDebugging()
{
	qDebug() <<__FUNCTION__<< CMD_SUSPEND_DEBUG;
	IpcSend(*m_clientConnection, CMD_SUSPEND_DEBUG);
}

// デバッグを再開
void CDebugger::resumeDebugging()
{
	qDebug() <<__FUNCTION__<< CMD_RESUME_DEBUG;
	IpcSend(*m_clientConnection, CMD_RESUME_DEBUG);
}

// デバッグを停止
void CDebugger::stopDebugging()
{
	qDebug() <<__FUNCTION__<< CMD_STOP_DEBUG;
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
	qDebug() <<__FUNCTION__<< CMD_REQ_VAR_INFO<< data;
	IpcSend(*m_clientConnection, CMD_REQ_VAR_INFO, data);
}

// ブレークポイントを更新
void CDebugger::updateBreakpoint()
{
	// メインのファイルが無題の場合は ???? からもUUIDを引けるようにする
	FileManager theFile_ = theFile;
	if( m_targetItem->isUntitled() ) {
		theFile_.assign("????", m_targetItem->uuid());
	}
	// ブレークポイントをDLL側へ送信
	QByteArray  data;
	QDataStream out(&data, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_4);
	out << theFile_ << theBreakPoint;
	qDebug() <<__FUNCTION__<< CMD_SET_BREAK_POINT << theFile << theBreakPoint;
	IpcSend(*m_clientConnection, CMD_SET_BREAK_POINT, data);
}
