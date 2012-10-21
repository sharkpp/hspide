#include <QFileInfo>
#include <QDebug>
#include "filemanager.h"

QDebug& operator<<(QDebug& debug, const FileManager& v)
{
	debug << v.m_manageInfo;
	return debug;
}

QDataStream& operator<<(QDataStream& stream, const FileManager& v)
{
	stream << v.m_manageInfo
	       << v.m_infoLookup;
	return stream;
}

QDataStream& operator>>(QDataStream& stream, FileManager& v)
{
	stream >> v.m_manageInfo
	       >> v.m_infoLookup;
	return stream;
}

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

FileManager::FileManager(const FileManager& rhs)
	: m_manageInfo(rhs.m_manageInfo)
	, m_infoLookup(rhs.m_infoLookup)
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
	return assign(filePath, QUuid::createUuid());
}

// ファイルパスにUUIDを割り当て
QUuid FileManager::assign(const QString& filePath, const QUuid& uuid)
{
	// すでに割り当て済みかチェック
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

// パスを取得
QString FileManager::path(const QUuid& uuid) const
{
    QMap<QUuid, QString>::const_iterator
		ite = m_manageInfo.find(uuid);
	if( m_manageInfo.end() != ite ) {
		return ite.value();
	}
#if defined(_DEBUG)
	qDebug() << __FUNCTION__ << uuid << "not found";
#endif
	return QString();
}

// ファイル名を取得
QString FileManager::fileName(const QUuid& uuid) const
{
	QFileInfo fileInfo = QFileInfo(path(uuid));
	return fileInfo.baseName().isEmpty() ? tr("(untitled)") : fileInfo.fileName();
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
#if defined(_DEBUG)
	qDebug() << __FUNCTION__ << filePath << "not found";
#endif
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
	QMap<QUuid, QString>::iterator
		ite = m_manageInfo.find(uuid);
	if( m_manageInfo.end() != ite ) {
		QMultiMap<QString, QUuid>::iterator
			ite2 = m_infoLookup.find(ite.value());
		if( m_infoLookup.end() != ite2 ) {
#if defined(_DEBUG)
			qDebug() << __FUNCTION__ << uuid << ite.value() << "->" << fileName;
#endif
			m_infoLookup.erase(ite2);
			m_infoLookup.insert(fileName, uuid);
			ite.value() = fileName;
			emit filePathChanged(uuid);
			return true;
		}
	}
	return !assign(fileName).isNull();
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

// 無題ファイル(ディスクに保存していないファイル)か？
bool FileManager::isUntitled(const QUuid& uuid) const
{
	return uuid.isNull() || QFileInfo(path(uuid)).baseName().isEmpty();
}
