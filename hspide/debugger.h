#include <QObject>
#include <QString>
#include <QProcess>
#include <QLocalSocket>
#include "debuggercommand.hpp"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CDebugger
	: public QObject
{
	Q_OBJECT

	QProcess*		m_compilerProcess;	// コンパイラプロセス

	QLocalSocket*	m_clientConnection;

	bool			m_waitAttach;

	CDebuggerCommand m_cmdQueue;

public:

	CDebugger(QObject *parent = 0);

	bool execCompiler(const QString & program, const QStringList & arguments, const QString & workDir);

	void setClientConnection(QLocalSocket* client);

	void setCommandQueue(CDebuggerCommand& cmdQueue);

	QProcess* compilerProcess();

	void standbyAttach();
	bool waitAttach();

protected:

	void parseCommand();

public slots:

	void compileError(QProcess::ProcessError);
	void compileFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void compileReadOutput();
	void recvCommand();

signals:

	void buildError(QProcess::ProcessError);
	void buildFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void buildReadOutput();

};
