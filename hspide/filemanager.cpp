#include <QFileInfo>
#include <QDebug>
#include "filemanager.h"

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

// �V�����t�@�C�����쐬
QUuid FileManager::create()
{
	return assign("");
}

// �t�@�C���p�X��UUID�����蓖��
QUuid FileManager::assign(const QString& filePath)
{
	return assign(filePath, QUuid::createUuid());
}

// �t�@�C���p�X��UUID�����蓖��
QUuid FileManager::assign(const QString& filePath, const QUuid& uuid)
{
	// ���łɊ��蓖�čς݂��`�F�b�N
	QList<QUuid> values = m_infoLookup.values(filePath);
	if( !values.empty() &&
		!filePath.isEmpty() )
	{
		return values.front();
	}

	m_manageInfo[uuid] = filePath;
	m_infoLookup.insert(filePath, uuid);
#if defined(_DEBUG)
	qDebug() << __FUNCTION__ << uuid << filePath;
#endif

	return uuid;
}

// �p�X���擾
QString FileManager::path(const QUuid& uuid) const
{
	QMap<QUuid, QString>::iterator
		ite = m_manageInfo.find(uuid);
	if( m_manageInfo.end() != ite ) {
		return ite.value();
	}
	return QString();
}

// �t�@�C�������擾
QString FileManager::fileName(const QUuid& uuid) const
{
	QFileInfo fileInfo = QFileInfo(path(uuid));
	return fileInfo.baseName().isEmpty() ? tr("(untitled)") : fileInfo.fileName();
}

// �t�@�C�������擾
QFileInfo FileManager::fileInfo(const QUuid& uuid)
{
	return QFileInfo(path(uuid));
}

// �p�X����UUID���擾
QUuid FileManager::uuid(const QString& filePath) const
{
	QList<QUuid> values = m_infoLookup.values(filePath);
	if( !values.empty() ) {
		return values.front();
	}
	return QUuid();
}

// �t�@�C��������UUID���擾
QUuid FileManager::uuidFromFilename(const QString& fileName)
{
	return QUuid();
}

// UUID���w�肵�Ċ֘A�t�����Ă���t�@�C������ύX
bool FileManager::rename(const QUuid& uuid, const QString& fileName)
{
	QMap<QUuid, QString>::iterator
		ite = m_manageInfo.find(uuid);
	if( m_manageInfo.end() != ite ) {
		QMultiMap<QString, QUuid>::iterator
			ite2 = m_infoLookup.find(ite.value());
		if( m_infoLookup.end() != ite2 ) {
			m_infoLookup.erase(ite2);
			m_infoLookup.insert(fileName, uuid);
			ite.value() = fileName;
			emit filePathChanged(uuid);
			return true;
		}
	}
	return false;
}

// UUID���w�肵�ēo�^�폜
bool FileManager::remove(const QUuid& uuid)
{
	return false;
}

// �S�ēo�^�폜
bool FileManager::removeAll()
{
	return false;
}

// ����t�@�C��(�f�B�X�N�ɕۑ����Ă��Ȃ��t�@�C��)���H
bool FileManager::isUntitled(const QUuid& uuid) const
{
	return uuid.isNull() || QFileInfo(path(uuid)).baseName().isEmpty();
}
