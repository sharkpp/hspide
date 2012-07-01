#include <QObject>
#include <QVector>
#include <QString>
#include <QProcess>
#include <QLocalSocket>
#include <QUuid>
#include "global.h"
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
	virtual ~CDebugger(){qDebug()<<__FUNCTION__<<this;}

	// �f�o�b�O�𒆒f
	void suspendDebugging();

	// �f�o�b�O���ĊJ
	void resumeDebugging();

	// �f�o�b�O���~
	void stopDebugging();

	// �X�e�b�vIn
	void stepIn();

	// �ϐ�����v��
	void reqVariableInfo(const QString& varName, int info[]);

	// �u���[�N�|�C���g���X�V
	void updateBreakpoint();

protected:

	void onRecvConnect(const QByteArray& param);
	void onRecvPutLog(const QByteArray& param);
	void onRecvStopRunning(const QByteArray& param);
	void onRecvUpdateDebugInfo(const QByteArray& param);
	void onRecvPutVarDigest(const QByteArray& param);
	void onRecvResVarInfo(const QByteArray& param);

public slots:

	void updateConfiguration(const Configuration& info);
	void recvCommand();

signals:

	void stopCode(const QUuid& uuid, int lineNo);
	void stopAtBreakPoint(const QUuid& uuid, int lineNo);
	void updateDebugInfo(const QVector<QPair<QString,QString> >& info);
	void updateVarInfo(const QVector<VARIABLE_INFO_TYPE>& info);
	void putLog(const QString& text);

};
