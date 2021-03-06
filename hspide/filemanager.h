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

	// 新しいファイルを作成
	QUuid create();

	// ファイルパスにUUIDを割り当て
	QUuid assign(const QString& filePath);

	// ファイルパスにUUIDを割り当て
	QUuid assign(const QString& filePath, const QUuid& uuid);

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
	
	// 無題ファイル(ディスクに保存していないファイル)か？
	bool isUntitled(const QUuid& uuid) const;

signals:

	void filePathChanged(const QUuid& uuid);
};

QDebug&      operator<<(QDebug& debug,       const FileManager& v);
QDataStream& operator<<(QDataStream& stream, const FileManager& v);
QDataStream& operator>>(QDataStream& stream,       FileManager& v);

#endif // !defined(INCLUDE_GUARD_1012AEB6_8EA6_4573_BACC_AE07F22F6131)
