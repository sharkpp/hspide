#include "workspacemodel.h"
#include "workspaceitem.h"

CWorkSpaceItem::CWorkSpaceItem(QObject * parent, CWorkSpaceModel * model)
	: QObject(parent)
	, m_model(model)
	, m_parent(NULL)
	, m_parentPos(0)
{
	m_icon = QIcon(":/images/tango/small/text-x-generic.png");
	m_text = QString("this=%1").arg((int)this, 0, 16);
}

CWorkSpaceItem::~CWorkSpaceItem()
{
	qDeleteAll(m_children);
}

const QString & CWorkSpaceItem::path() const
{
	return m_path;
}

void CWorkSpaceItem::setPath(const QString & path)
{
	m_path = path;
}

const QString & CWorkSpaceItem::text() const
{
	return m_text;
}

void CWorkSpaceItem::setText(const QString & text)
{
	m_text = text;
}

const QIcon & CWorkSpaceItem::icon() const
{
	return m_icon;
}

void CWorkSpaceItem::setIcon(const QIcon & icon)
{
	m_icon = icon;
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
	if( m_model ) {
		return m_model->createIndex(m_parentPos, 0, (void*)this);
	}
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

