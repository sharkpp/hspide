#include <QFileInfo>
#include "workspaceitem.h"
#include "workspacemodel.h"

CWorkSpaceItem::CWorkSpaceItem(QObject * parent, CWorkSpaceModel * model)
	: QObject(parent)
	, m_model(model)
	, m_parent(NULL)
	, m_parentPos(0)
{
}

CWorkSpaceItem::~CWorkSpaceItem()
{
	qDeleteAll(m_children);
}

int CWorkSpaceItem::count() const
{
	return m_children.size();
}

CWorkSpaceItem * CWorkSpaceItem::at(int index) const
{
	return
		0 <= index && index < m_children.size()
			? m_children[index]
			: NULL;
}

CWorkSpaceItem * CWorkSpaceItem::parent() const
{
	return m_parent;
}

int CWorkSpaceItem::parentPosition() const
{
	return m_parentPos;
}

QModelIndex CWorkSpaceItem::index() const
{
	return QModelIndex();
}

bool CWorkSpaceItem::insert(int position, CWorkSpaceItem * item)
{
	if( position < 0 || m_children.size() < position ) {
		return false;
	}

	item->m_parentPos = position;
	item->m_parent    = this;
	item->m_model     = m_model;

	m_children.insert(position, item);

	return true;
}

