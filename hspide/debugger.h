#include <QObject>
#include <QString>
#include <QProcess>
#include <QLocalSocket>
#include <QUuid>
#include "debuggercommand.hpp"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CDebugger
	: public QObject
{
	Q_OBJECT

	QLocalSocket*	m_clientConnection;

	bool			m_waitAttach;

	CDebuggerCommand m_cmdQueue;

public:

	CDebugger(QObject *parent, QLocalSocket* socket);

	// �f�o�b�O�𒆒f
	void suspendDebugging();

	// �f�o�b�O���~
	void stopDebugging();

protected:

	void parseCommand();

public slots:

	void recvCommand();

signals:

	void stopAtBreakPoint(const QUuid & uuid, int lineNum);

};
