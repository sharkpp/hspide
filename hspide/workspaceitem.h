#include <QObject>
#include <QModelIndex>
#include <QList>
#include <QString>
#include <QIcon>

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
	} Type;

private:

	CWorkSpaceModel*		m_model;
	CWorkSpaceItem*			m_parent;
	QList<CWorkSpaceItem*>	m_children;
	int						m_parentPos;
	QIcon					m_icon;
	QString					m_text;
	QString					m_path;
	Type					m_type;
	NodeType				m_nodeType;

	CDocumentPane*			m_assignDocument;

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

	bool load(const QString & fileName);
	bool save(const QString & fileName = QString());

	bool isUntitled() const;

	// アイテムと関連付け
	bool setAssignDocument(CDocumentPane * item);
	CDocumentPane * assignDocument();

	int count() const;
	CWorkSpaceItem * at(int index) const;
	CWorkSpaceItem * parent() const;
	CWorkSpaceItem * ancestor(Type type);
	CWorkSpaceItem * search(const QString & path);
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

