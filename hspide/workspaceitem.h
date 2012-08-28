#include <QObject>
#include <QModelIndex>
#include <QList>
#include <QSet>
#include <QString>
#include <QIcon>
#include <QUuid>
#include "global.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CWorkSpaceModel;
class CDocumentPane;
class QXmlStreamWriter;
class QXmlStreamReader;

class CWorkSpaceItem
	: public QObject
{
	Q_OBJECT

public:

	typedef enum {
		UnkownNodeType = 0,
		FileNode,
		FolderNode,
		NodeTypeNum,
	} NodeType;

	typedef enum {
		UnkownType = 0,
		File,
		Folder,
		Solution,
		Project,
		DependenceFolder,
		PackFolder,
		SourceFolder,
		ResourceFolder,
		TypeNum,
	} Type;

	typedef enum {
		WithoutSave,
		OverwriteSave,
		SaveAs,
	} SaveType;

	enum {
		UuidRole = Qt::UserRole + 2,
		TypeRole,
		NodeTypeRole,
		SuffixFilterRole,
	};

private:

	CWorkSpaceModel*		m_model;			// モデル
	CWorkSpaceItem*			m_parent;			// 親アイテム
	QList<CWorkSpaceItem*>	m_children;			// 子アイテム
	QIcon					m_icon;				// 表示アイコン
	QString					m_text;				// 表示テキスト
	Type					m_type;				// 種別
	NodeType				m_nodeType;			// ノード種別
	QUuid					m_uuid;				// ファイルに対して一意に割り当てられているID

	QStringList				m_suffixFilters;	// 拡張子のフィルタ

	Configuration::BuildConfList	
							m_buildConfigurations;		// 有効なビルドターゲット
	QUuid					m_buildTarget;

	CDocumentPane*			m_assignDocument;	// 関連付けられているドキュメント

public:

	CWorkSpaceItem(QObject* parent, CWorkSpaceModel* model = NULL);
	CWorkSpaceItem(QObject* parent, Type type, CWorkSpaceModel* model = NULL);
	~CWorkSpaceItem();

	CWorkSpaceModel* model() const;
	void setModel(CWorkSpaceModel* model);

	Type type() const;
	void setType(Type type);

	NodeType nodeType() const;
	void setNodeType(NodeType type);

	QString path() const;
//	void setPath(const QString& path);

	const QString& text() const;
	void setText(const QString& text = QString());

	const QIcon& icon() const;
	void setIcon(const QIcon& icon);

	const QStringList& suffixFilter() const;
	void setSuffixFilter(const QStringList& filter);

	bool load(const QString& fileName);
	bool save(const QString& fileName = QString(), SaveType saveType = OverwriteSave, bool noReclusive = false);
	bool save(SaveType saveType, bool noReclusive = false);

	bool isUntitled() const;

	const QUuid& uuid() const;
	void setUuid(const QUuid& uuid);

	const Configuration::BuildConfList& buildConf() const;
	void  setBuildConf(const Configuration::BuildConfList& conf);
	const QUuid& buildTarget() const;
	void  setBuildTarget(const QUuid& target);
	const Configuration::BuildConfType& currentBuildConf() const;

	// アイテムと関連付け
	bool setAssignDocument(CDocumentPane* item);
	CDocumentPane* assignDocument();

	int count() const;
	CWorkSpaceItem* at(int index) const;
	CWorkSpaceItem* parent() const;
	CWorkSpaceItem* root() const;
	CWorkSpaceItem* ancestor(Type type);
	CWorkSpaceItem* search(const QString& path, bool basename = false);
	CWorkSpaceItem* search(const QUuid& path);
	int parentPosition() const;
	QModelIndex index() const;
	bool insert(int position, CWorkSpaceItem* item);
	bool remove(int position);
	QList<CWorkSpaceItem*> take();

private:

	bool saveSolution(const QString& fileName, bool saveAs = false);
	bool loadSolution(const QString& fileName);

	bool serialize(QXmlStreamWriter* xml);
	bool deserialize(QXmlStreamReader* xml);

	void updatePath();

public slots:

	void onFileChanged(const QUuid& uuid);

signals:

	void loadComplete();
	void saveComplete();
};

