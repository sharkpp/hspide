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
	// �ݒ�X�V
	void setConfiguration(const Configuration& info);

	// �f�o�b�O�𒆒f
	void suspendDebugging();

	// �f�o�b�O���ĊJ
	void resumeDebugging();

	// �f�o�b�O���~
	void stopDebugging();

	// �ϐ�����v��
	void reqVariableInfo(const QString& varName, int info[]);

	// �u���[�N�|�C���g���X�V
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
