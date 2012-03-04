#include <QObject>
#include <QModelIndex>
#include <QList>
#include <QSet>
#include <QString>
#include <QIcon>
#include <QUuid>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CWorkSpaceModel;
class CDocumentPane;
class QXmlStreamWriter;
class QXmlStreamReader;

typedef QMap<QUuid, QSet<int> > CBreakPointInfo;
typedef QMap<QString, QUuid> CUuidLookupInfo;

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

	enum {
		PathRole = Qt::UserRole + 2,
		TypeRole,
		NodeTypeRole,
		UuidRole,
		SuffixFilterRole,
	};

private:

	CWorkSpaceModel*		m_model;			// モデル
	CWorkSpaceItem*			m_parent;			// 親アイテム
	QList<CWorkSpaceItem*>	m_children;			// 子アイテム
	int						m_parentPos;		// 親アイテムでの位置
	QIcon					m_icon;				// 表示アイコン
	QString					m_text;				// 表示テキスト
	QString					m_path;				// ファイルパス
	Type					m_type;				// 種別
	NodeType				m_nodeType;			// ノード種別
	QUuid					m_uuid;				// ファイルに対して一意に割り当てられているID

	QStringList				m_suffixFilters;	// 拡張子のフィルタ

	CDocumentPane*			m_assignDocument;	// 関連付けられているドキュメント

	QSet<int>				m_breakpoints;		// ブレークポイント

public:

	CWorkSpaceItem(QObject * parent, CWorkSpaceModel * model = NULL);
	CWorkSpaceItem(QObject * parent, Type type, CWorkSpaceModel * model = NULL);
	~CWorkSpaceItem();

	CWorkSpaceModel* model() const;
	void setModel(CWorkSpaceModel* model);

	Type type() const;
	void setType(Type type);

	NodeType nodeType() const;
	void setNodeType(NodeType type);

	const QString & path() const;
	void setPath(const QString & path);

	const QString & text() const;
	void setText(const QString & text = QString());

	const QIcon & icon() const;
	void setIcon(const QIcon & icon);

	const QStringList & suffixFilter() const;
	void setSuffixFilter(const QStringList & filter);

	bool load(const QString & fileName);
	bool save(const QString & fileName = QString());

	bool isUntitled() const;

	const QUuid & uuid() const;
	void setUuid(const QUuid & uuid);

	void setBreakPoint(int lineNo);
	void clearBreakPoint(int lineNo);
	bool getBreakPoints(CBreakPointInfo & breakpoints, CUuidLookupInfo & lookup);

	// アイテムと関連付け
	bool setAssignDocument(CDocumentPane * item);
	CDocumentPane * assignDocument();

	int count() const;
	CWorkSpaceItem * at(int index) const;
	CWorkSpaceItem * parent() const;
	CWorkSpaceItem * ancestor(Type type);
	CWorkSpaceItem * search(const QString & path, bool basename = false);
	CWorkSpaceItem * search(const QUuid & path);
	int parentPosition() const;
	QModelIndex index() const;
	bool insert(int position, CWorkSpaceItem * item);
	bool remove(int position);

private:

	bool saveSolution(const QString & fileName);
	bool loadSolution(const QString & fileName);

	bool serialize(QXmlStreamWriter * xml);
	bool deserialize(QXmlStreamReader * xml);
};

