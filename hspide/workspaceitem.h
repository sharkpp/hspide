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

	CWorkSpaceModel*		m_model;
	CWorkSpaceItem*			m_parent;
	QList<CWorkSpaceItem*>	m_children;
	int						m_parentPos;
	QIcon					m_icon;
	QString					m_text;
	QString					m_path;

public:

	CWorkSpaceItem(QObject * parent, CWorkSpaceModel * model = NULL);
	~CWorkSpaceItem();

	const QString & path() const;
	void setPath(const QString & path);

	const QString & text() const;
	void setText(const QString & text);

	const QIcon & icon() const;
	void setIcon(const QIcon & icon);

	int count() const;
	CWorkSpaceItem * at(int index) const;
	CWorkSpaceItem * parent() const;
	int parentPosition() const;
	QModelIndex index() const;
	bool insert(int position, CWorkSpaceItem * item);
};

