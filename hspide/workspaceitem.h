#include <QObject>
#include <QString>
#include <QStandardItem>
#include <QIcon>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CWorkSpaceModel;

class CWorkSpaceItem
	: QObject
{
	Q_OBJECT

	CWorkSpaceModel* m_model;
	CWorkSpaceItem*	 m_parent;
	QVector<CWorkSpaceItem*> m_children;
	int m_parentPos;

public:

	CWorkSpaceItem(QObject * parent, CWorkSpaceModel * model = NULL);
	~CWorkSpaceItem();

	int count() const;
	CWorkSpaceItem * at(int index) const;
	CWorkSpaceItem * parent() const;
	int parentPosition() const;
	QModelIndex index() const;
	bool insert(int position, CWorkSpaceItem * item);
};

