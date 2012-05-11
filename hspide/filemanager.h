#include <QObject>
#include <QFileInfo>
#include <QMap>
#include <QString>
#include <QUuid>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class FileManager
	: public QObject
{
	Q_OBJECT

	QMap<QUuid, QString>		m_manageInfo;
	QMultiMap<QString, QUuid>	m_infoLookup;

public:

	FileManager();
	~FileManager();

	// �V�����t�@�C�����쐬
	QUuid create();

	// �t�@�C���p�X��UUID�����蓖��
	QUuid assign(const QString& filePath);

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

private:

};

