#include <QObject>
#include <QVector>
#include <QString>
#include <QProcess>
#include <QLocalSocket>
#include <QUuid>
#include "configuration.h"
#include "ipc_common.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CWorkSpaceItem;

class CDebugger
	: public QObject
{
	Q_OBJECT

	QLocalSocket*	m_clientConnection;

	bool			m_waitAttach;

	QVector<QPair<QString,QString> >	m_debugInfo;

	CWorkSpaceItem*	m_targetItem;

public:

	CDebugger(QObject *parent, QLocalSocket* socket);
virtual ~CDebugger();
	// 設定更新
	void setConfiguration(const Configuration& info);

	// デバッグを中断
	void suspendDebugging();

	// デバッグを再開
	void resumeDebugging();

	// デバッグを停止
	void stopDebugging();

	// 変数情報を要求
	void reqVariableInfo(const QString& varName, int info[]);

	// ブレークポイントを更新
	void updateBreakpoint();

protected:

public slots:

	void updateConfiguration(const Configuration& info);
	void recvCommand();

signals:

	void stopAtBreakPoint(const QUuid & uuid, int lineNo);
	void updateDebugInfo(const QVector<QPair<QString,QString> > & info);
	void updateVarInfo(const QVector<VARIABLE_INFO_TYPE> & info);
	void putLog(const QString& text);

};
