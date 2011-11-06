#include <QObject>
#include <QModelIndex>
#include <QList>
#include <QString>
#include <QIcon>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CWorkSpaceModel;

class CWorkSpaceItem
	: QObject
{
	Q_OBJECT

public:

	typedef enum {
		UnkownType = 0,
		File,
		Folder,
	} Type;

	typedef enum {
		UnkownSubType = 0,
		Default,
		Solution,
		Project,
		DependenceFolder,
		PackFolder,
		SourceFolder,
		ResourceFolder,
	} SubType;

private:

	CWorkSpaceModel*		m_model;
	CWorkSpaceItem*			m_parent;
	QList<CWorkSpaceItem*>	m_children;
	int						m_parentPos;
	QIcon					m_icon;
	QString					m_text;
	QString					m_path;
	Type					m_type;
	SubType					m_subType;

public:

	CWorkSpaceItem(QObject * parent, CWorkSpaceModel * model = NULL);
	~CWorkSpaceItem();

	Type type() const;
	void setType(Type type);

	SubType subType() const;
	void setSubType(SubType type);

	const QString & path() const;
	void setPath(const QString & path);

	const QString & text() const;
	void setText(const QString & text = QString());

	const QIcon & icon() const;
	void setIcon(const QIcon & icon);

	int count() const;
	CWorkSpaceItem * at(int index) const;
	CWorkSpaceItem * parent() const;
	int parentPosition() const;
	QModelIndex index() const;
	bool insert(int position, CWorkSpaceItem * item);
};

