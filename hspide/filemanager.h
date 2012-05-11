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

	// 新しいファイルを作成
	QUuid create();

	// ファイルパスにUUIDを割り当て
	QUuid assign(const QString& filePath);

	// パスを取得
	QString path(const QUuid& uuid) const;

	// ファイル名を取得
	QString fileName(const QUuid& uuid) const;

	// ファイル情報を取得
	QFileInfo fileInfo(const QUuid& uuid);

	// パスからUUIDを取得
	QUuid uuid(const QString& filePath) const;

	// ファイル名からUUIDを取得
	QUuid uuidFromFilename(const QString& fileName);

	// UUIDを指定して関連付けられているファイル名を変更
	bool rename(const QUuid& uuid, const QString& fileName);

	// UUIDを指定して登録削除
	bool remove(const QUuid& uuid);
	
	// 全て登録削除
	bool removeAll();

private:

};

