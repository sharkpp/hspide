#include <QFileInfo>
#include <QDebug>
#include "filemanager.h"

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

// 新しいファイルを作成
QUuid FileManager::create()
{
	return assign("");
}

// ファイルパスにUUIDを割り当て
QUuid FileManager::assign(const QString& filePath)
{
	QUuid uuid(QUuid::createUuid());
	m_manageInfo[uuid] = filePath;
	m_infoLookup.insert(filePath, uuid);
#if defined(_DEBUG)
	qDebug() << __FUNCTION__ << uuid << filePath;
#endif
	return uuid;
}

// パスを取得
QString FileManager::path(const QUuid& uuid) const
{
	QMap<QUuid, QString>::iterator
		ite = m_manageInfo.find(uuid);
	if( m_manageInfo.end() != ite ) {
		return ite.value();
	}
	return QString();
}

// ファイル名を取得
QString FileManager::fileName(const QUuid& uuid) const
{
	QString fileName = QFileInfo(path(uuid)).fileName();
	return fileName.isEmpty() ? tr("(untitled)") : fileName;
}

// ファイル情報を取得
QFileInfo FileManager::fileInfo(const QUuid& uuid)
{
	return QFileInfo(path(uuid));
}

// パスからUUIDを取得
QUuid FileManager::uuid(const QString& filePath) const
{
	QList<QUuid> values = m_infoLookup.values(filePath);
	if( !values.empty() ) {
		return values.front();
	}
	return QUuid();
}

// ファイル名からUUIDを取得
QUuid FileManager::uuidFromFilename(const QString& fileName)
{
	return QUuid();
}

// UUIDを指定して関連付けられているファイル名を変更
bool FileManager::rename(const QUuid& uuid, const QString& fileName)
{
	return false;
}

// UUIDを指定して登録削除
bool FileManager::remove(const QUuid& uuid)
{
	return false;
}

// 全て登録削除
bool FileManager::removeAll()
{
	return false;
}

