#include <QDebug>
#include <QTextCodec>
#include "debugger.h"

CDebugger::CDebugger(QObject *parent)
	: QObject(parent)
	, m_compilerProcess(NULL)
	, m_clientConnection(NULL)
	, m_waitAttach(false)
{
	m_compilerProcess = new QProcess(this);
}

void CDebugger::standbyAttach()
{
	m_waitAttach = true;
}

bool CDebugger::waitAttach()
{
	return m_waitAttach;
}

void CDebugger::setClientConnection(QLocalSocket* client)
{
	m_clientConnection = client;

	connect(m_clientConnection, SIGNAL(readyRead()), this, SLOT(recvCommand()));
}

void CDebugger::setCommandQueue(CDebuggerCommand& cmdQueue)
{
	m_cmdQueue.swap(cmdQueue);

	parseCommand();
}

bool CDebugger::execCompiler(const QString & program, const QStringList & arguments, const QString & workDir)
{
	m_compilerProcess->setWorkingDirectory(workDir);
	m_compilerProcess->start(program, arguments);

	if( !m_compilerProcess->waitForStarted() ) {
		delete m_compilerProcess;
		m_compilerProcess = NULL;
		return false;
	}

	// ˆ—Š®—¹Žž‚Ì’Ê’m‚ð“o˜^
	connect(m_compilerProcess, SIGNAL(error(QProcess::ProcessError)),      this, SLOT(compileError(QProcess::ProcessError)));
	connect(m_compilerProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(compileFinished(int,QProcess::ExitStatus)));
	connect(m_compilerProcess, SIGNAL(readyReadStandardOutput()),          this, SLOT(compileReadOutput()));

	return true;
}

QProcess* CDebugger::compilerProcess()
{
	return m_compilerProcess;
}

void CDebugger::parseCommand()
{
	quint64 id;
	quint8  cmd_id;
	quint32 length;

	for(;;)
	{
		CDebuggerCommand::scoped_ptr ptr = m_cmdQueue.read(id, cmd_id, length);
		if( !ptr.valid() )
		{
			break;
		}

		switch(cmd_id)
		{
		case 0x01: {
			QTextCodec* codec = QTextCodec::codecForLocale();
			QString s = codec->toUnicode(QByteArray((const char*)ptr.data(), ptr.size()));
			qDebug() <<"CDebugger::recvCommand"<< id << cmd_id << length << s;
			break; }
		default:
			qDebug() <<"CDebugger::recvCommand"<< id << cmd_id;
		}
	}
}

void CDebugger::compileError(QProcess::ProcessError error)
{
	emit buildError(error);
}

void CDebugger::compileFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	emit buildFinished(exitCode, exitStatus);
}

void CDebugger::compileReadOutput()
{
	emit buildReadOutput();
}

void CDebugger::recvCommand()
{
	QByteArray data = m_clientConnection->readAll();
	m_cmdQueue.push(data.data(), data.size());
	parseCommand();
}
