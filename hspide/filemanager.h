#include <QObject>
#include <QFileInfo>
#include <QMap>
#include <QString>
#include <QUuid>
#include <QDebug>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef INCLUDE_GUARD_1012AEB6_8EA6_4573_BACC_AE07F22F6131
#define INCLUDE_GUARD_1012AEB6_8EA6_4573_BACC_AE07F22F6131

class FileManager
	: public QObject
{
	Q_OBJECT

	friend QDebug&      operator<<(QDebug& debug,       const FileManager& v);
	friend QDataStream& operator<<(QDataStream& stream, const FileManager& v);
	friend QDataStream& operator>>(QDataStream& stream,       FileManager& v);

	QMap<QUuid, QString>		m_manageInfo;
	QMultiMap<QString, QUuid>	m_infoLookup;

public:

	FileManager();
	~FileManager();

	FileManager(const FileManager& rhs);

	// �V�����t�@�C�����쐬
	QUuid create();

	// �t�@�C���p�X��UUID�����蓖��
	QUuid assign(const QString& filePath);

	// �t�@�C���p�X��UUID�����蓖��
	QUuid assign(const QString& filePath, const QUuid& uuid);

	// �p�X���擾
	QString path(const QUuid& uuid) const;

	// �t�@�C�������擾
	QString fileName(const QUuid& uuid) const;

	// �t�@�C�������擾
	QFileInfo fileInfo(const QUuid& uuid);

	// �p�X����UUID���擾
	QUuid uuid(const QString& filePath) const;

	// �t�@�C��������UUID���擾
	QUuid uuidFromFilename(const QString& fileName);

	// UUID���w�肵�Ċ֘A�t�����Ă���t�@�C������ύX
	bool rename(const QUuid& uuid, const QString& fileName);

	// UUID���w�肵�ēo�^�폜
	bool remove(const QUuid& uuid);
	
	// �S�ēo�^�폜
	bool removeAll();
	
	// ����t�@�C��(�f�B�X�N�ɕۑ����Ă��Ȃ��t�@�C��)���H
	bool isUntitled(const QUuid& uuid) const;

signals:

	void filePathChanged(const QUuid& uuid);
};

QDebug&      operator<<(QDebug& debug,       const FileManager& v);
QDataStream& operator<<(QDataStream& stream, const FileManager& v);
QDataStream& operator>>(QDataStream& stream,       FileManager& v);

#endif // !defined(INCLUDE_GUARD_1012AEB6_8EA6_4573_BACC_AE07F22F6131)
