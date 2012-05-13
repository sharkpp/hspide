#include <QApplication>
#include <QFileInfo>
#include <QFileDialog>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QTreeView>
#include "workspacemodel.h"
#include "workspaceitem.h"
#include "documentpane.h"
#include "global.h"

CWorkSpaceItem::CWorkSpaceItem(QObject * parent, CWorkSpaceModel * model)
	: QObject(parent)
	, m_model(model)
	, m_parent(NULL)
	, m_parentPos(0)
	, m_type(UnkownType)
	, m_nodeType(UnkownNodeType)
	, m_assignDocument(NULL)
{
	m_icon = QIcon(":/images/tango/small/text-x-generic.png");
	m_text = QString("this=%1").arg((int)this, 0, 16);

	if( !model )
	{
		if( CWorkSpaceItem* parentItem = qobject_cast<CWorkSpaceItem*>(parent) )
		{
			m_model = parentItem->model();
		}
		else if( CWorkSpaceModel* parentModel = qobject_cast<CWorkSpaceModel*>(parent) )
		{
			m_model = parentModel;
		}
		else if( QTreeView* parentWidget = qobject_cast<QTreeView*>(parent) )
		{
			if( CWorkSpaceModel* parentModel = qobject_cast<CWorkSpaceModel*>(parentWidget->model()) )
			{
				m_model = parentModel;
			}
		}
	}

	// �ʒm��o�^
	connect(&theFile, SIGNAL(filePathChanged(QUuid)), this, SLOT(onFileChanged(QUuid)));
}

CWorkSpaceItem::CWorkSpaceItem(QObject * parent, Type type, CWorkSpaceModel * model)
	: QObject(parent)
	, m_model(NULL)
	, m_parent(NULL)
	, m_parentPos(0)
	, m_type(type)
	, m_assignDocument(NULL)
{
	static const struct {
		NodeType nodeType;
		QString iconPath;
		QString text;
		QString suffixFilter;
	} TypeDefaultAttributes[TypeNum] = {
		{ UnkownNodeType, ":/images/tango/small/text-x-generic.png", "",               "",                                    }, // UnkownType
		{ FileNode,       ":/images/tango/small/text-x-generic.png", tr("(untitled)"), "",                                    }, // File
		{ FolderNode,     ":/images/tango/small/folder.png",         tr("(untitled)"), "",                                    }, // Folder
		{ FolderNode,     ":/images/tango/small/edit-copy.png",      tr("(untitled)"), "",                                    }, // Solution
		{ FileNode,       ":/images/tango/small/folder.png",         tr("(untitled)"), "*.hsp;*.as",                          }, // Project
		{ FolderNode,     ":/images/tango/small/folder.png",         tr("Dependence"), "",                                    }, // DependenceFolder
		{ FolderNode,     ":/images/tango/small/folder.png",         tr("Packfile"),   "*.*",                                 }, // PackFolder
		{ FolderNode,     ":/images/tango/small/folder.png",         tr("Source"),     "*.hsp;*.as",                          }, // SourceFolder
		{ FolderNode,     ":/images/tango/small/folder.png",         tr("Resource"),   "*.png;*.jpg;*.bmp;*.gif;*,wav;*.mid", }, // ResourceFolder
	};

	// ��ʂɉ����đ������Z�b�g
	setNodeType(    TypeDefaultAttributes[type].nodeType);
	setIcon(QIcon(  TypeDefaultAttributes[type].iconPath));
	setText(        TypeDefaultAttributes[type].text);
	setSuffixFilter(TypeDefaultAttributes[type].suffixFilter.split(";"));

	m_model = model;

	if( !model )
	{
		if( CWorkSpaceItem* parentItem = qobject_cast<CWorkSpaceItem*>(parent) )
		{
			m_model = parentItem->model();
		}
		else if( CWorkSpaceModel* parentModel = qobject_cast<CWorkSpaceModel*>(parent) )
		{
			m_model = parentModel;
		}
		else if( QTreeView* parentWidget = qobject_cast<QTreeView*>(parent) )
		{
			if( CWorkSpaceModel* parentModel = qobject_cast<CWorkSpaceModel*>(parentWidget->model()) )
			{
				m_model = parentModel;
			}
		}
	}

	// �ʒm��o�^
	connect(&theFile, SIGNAL(filePathChanged(QUuid)), this, SLOT(onFileChanged(QUuid)));
}

CWorkSpaceItem::~CWorkSpaceItem()
{
	qDeleteAll(m_children);
}

CWorkSpaceModel* CWorkSpaceItem::model() const
{
	return m_model;
}

void CWorkSpaceItem::setModel(CWorkSpaceModel* model)
{
	m_model = model;
}

CWorkSpaceItem::Type CWorkSpaceItem::type() const
{
	return m_type;
}

void CWorkSpaceItem::setType(Type type)
{
	m_type = type;

	// ���f���ɍX�V��ʒm���r���[���ĕ`��
	if( m_model ) {
		m_model->dataChanged(index(), index());
	}
}

CWorkSpaceItem::NodeType CWorkSpaceItem::nodeType() const
{
	return m_nodeType;
}

void CWorkSpaceItem::setNodeType(NodeType type)
{
	m_nodeType = type;

	// ���f���ɍX�V��ʒm���r���[���ĕ`��
	if( m_model ) {
		m_model->dataChanged(index(), index());
	}
}

// �t�@�C���p�X���擾
QString CWorkSpaceItem::path() const
{
	return theFile.path(m_uuid);
}

const QString& CWorkSpaceItem::text() const
{
	return m_text;
}

void CWorkSpaceItem::setText(const QString & text)
{
	m_text = text;

	// ���f���ɍX�V��ʒm���r���[���ĕ`��
	if( m_model ) {
		m_model->dataChanged(index(), index());
	}
}

const QIcon & CWorkSpaceItem::icon() const
{
	return m_icon;
}

void CWorkSpaceItem::setIcon(const QIcon & icon)
{
	m_icon = icon;

	// ���f���ɍX�V��ʒm���r���[���ĕ`��
	if( m_model ) {
		m_model->dataChanged(index(), index());
	}
}

const QStringList & CWorkSpaceItem::suffixFilter() const
{
	return m_suffixFilters;
}

void CWorkSpaceItem::setSuffixFilter(const QStringList & filters)
{
	m_suffixFilters = filters;

	// ���f���ɍX�V��ʒm���r���[���ĕ`��
	if( m_model ) {
		m_model->dataChanged(index(), index());
	}
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

CWorkSpaceItem * CWorkSpaceItem::ancestor(Type type)
{
	return m_parent ? type == m_type ? this
	                                 : m_parent->ancestor(type)
	                : NULL;
}

CWorkSpaceItem * CWorkSpaceItem::search(const QString & path, bool basename)
{
	QString comp = basename ? QFileInfo(m_path).baseName()
							: m_path;

	if( (FileNode == m_nodeType || !path.isEmpty()) &&
		!comp.compare(path, Qt::CaseSensitive) )
	{
		return this;
	}

	foreach(CWorkSpaceItem* item, m_children)
	{
		CWorkSpaceItem* result = item->search(path, basename);
		if( result )
		{
			return result;
		}
	}

	return NULL;
}

CWorkSpaceItem * CWorkSpaceItem::search(const QUuid & uuid)
{
	if( uuid == m_uuid )
	{
		return this;
	}

	foreach(CWorkSpaceItem* item, m_children)
	{
		CWorkSpaceItem* result = item->search(uuid);
		if( result )
		{
			return result;
		}
	}

	return NULL;
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

bool CWorkSpaceItem::remove(int position)
{
	if( position < 0 || m_children.size() < position ) {
		return false;
	}

	m_children.removeAt(position);

	return true;
}

// ����t�@�C��(�f�B�X�N�ɕۑ����Ă��Ȃ��t�@�C��)���H
bool CWorkSpaceItem::isUntitled() const
{
	return theFile.isUntitled(m_uuid);
}

const QUuid & CWorkSpaceItem::uuid() const
{
	return m_uuid;
}

void CWorkSpaceItem::setUuid(const QUuid & uuid)
{
	m_uuid = uuid;
}

void CWorkSpaceItem::setBreakPoint(int lineNo)
{
	m_breakpoints.insert(lineNo);
}

void CWorkSpaceItem::clearBreakPoint(int lineNo)
{
	m_breakpoints.remove(lineNo);
}

bool CWorkSpaceItem::getBreakPoints(CBreakPointInfo & breakpoints, CUuidLookupInfo & lookup)
{
	QString filename = QFileInfo(m_path).baseName();

	breakpoints[m_uuid] = m_breakpoints;
	lookup[!filename.compare("???") ? "" : filename] = m_uuid;

	// �q�̃u���C�N�|�C���g����
	foreach(CWorkSpaceItem* item, m_children)
	{
		switch(item->m_type)
		{
		case File:
		case Solution:
		case Project:
			item->getBreakPoints(breakpoints, lookup);
			break;
		}
	}

	return true;
}

bool CWorkSpaceItem::load(const QString & fileName)
{
	if( m_assignDocument )
	{
		if( m_assignDocument->load(fileName) )
		{
		}
	}
	else
	{
		switch(m_type)
		{
		case Solution:
			if( m_model ) {
				if( count() ) {
					m_model->removeRows(0, count(), index());
				}
			} else {
				qDeleteAll(m_children);
			}
			loadSolution(fileName);
			break;
		}
	}

	return true;
}

bool CWorkSpaceItem::save(const QString & fileName)
{
	// �q���S�ĕۑ�
	foreach(CWorkSpaceItem* item, m_children)
	{
		if( item->assignDocument() &&
			item->assignDocument()->isModified() )
		{
			item->save();
		}
	}

	if( m_assignDocument )
	{
		m_assignDocument->save(fileName);
	}
	else
	{
		switch(m_type)
		{
		case Solution:
			saveSolution(fileName);
			break;
		}
	}

	return true;
}

bool CWorkSpaceItem::saveSolution(const QString & fileName)
{
	QXmlStreamWriter xml;
	QString filePath = fileName;

	// �t�@�C�������w�肳��Ă��Ȃ��ꍇ�̓_�C�A���O��\�����ĕ\��
	if( filePath.isEmpty() )
	{
		filePath = QFileDialog::getSaveFileName(QApplication::activeWindow(),
						tr("Save File"), tr("untitled") + ".hspsln",
						tr("HSP IDE Solution File (*.hspsln)") + ";;" +
						tr("All File (*.*)")
						);
		if( filePath.isEmpty() )
		{
			return false;
		}
	}

	// �������ݗp�̃t�@�C�����I�[�v��
	QFile file(filePath);
	if( !file.open(QFile::WriteOnly | QFile::Text) )
	{
		return false;
	}

	theFile.rename(m_uuid, filePath);

	// �t�@�C����XML�Ƃ��ďo��
	xml.setDevice(&file);
	xml.writeStartDocument();
//	xml.writeDTD("<!DOCTYPE hspide>");
	xml.writeStartElement("hspide");
	xml.writeAttribute("version", "1.0");
	xml.writeAttribute("type", "solution");

	// �A�C�e���c���[��XML�ɕϊ�
	serialize(&xml);

	xml.writeEndDocument();

	return true;
}

bool CWorkSpaceItem::loadSolution(const QString & fileName)
{
	// �ǂݍ��ݗp�̃t�@�C�����I�[�v��
	QFile file(fileName);
	if( !file.open(QFile::ReadOnly | QFile::Text) )
	{
		return false;
	}

	QXmlStreamReader xml(&file);
	QVector<CWorkSpaceItem*> vItems;

	// XML���I���܂œǂݍ���
	while( !xml.atEnd() &&
	       !xml.hasError() )
	{
		QXmlStreamReader::TokenType token = xml.readNext();

		switch( token )
		{
		case QXmlStreamReader::StartDocument:
			// ��ʂ��`�F�b�N
			if( !xml.attributes().value("type").compare("solution", Qt::CaseSensitive) )
			{
				xml.clear();
				return false;
			}
			// ���[�g���w��
			vItems.push_back(this);
			if( m_model ) {
				m_model->setData(index(), text(), Qt::DisplayRole);
			}
			break;
		case QXmlStreamReader::StartElement: {
			CWorkSpaceItem* parentItem = vItems.back();
			CWorkSpaceItem* newItem    = NULL;
			QString name = xml.name().toString();
			QString path;
			/*****/if( !name.compare("solution", Qt::CaseSensitive) ) {
				if( 1 != vItems.size() ) {
					xml.clear();
					return false;
				}
				newItem = this;
				path = fileName;
			//	setText(xml.attributes().value("name").toString());
			//	continue;
			} else if( !name.compare("project", Qt::CaseSensitive) ) {
				newItem = new CWorkSpaceItem(parentItem, Project);
				path = xml.attributes().value("path").toString();
			} else if( !name.compare("folder", Qt::CaseSensitive) ) {
				newItem = new CWorkSpaceItem(parentItem, Folder);
				newItem->setText(xml.attributes().value("name").toString());
			} else if( !name.compare("file", Qt::CaseSensitive) ) {
				newItem = new CWorkSpaceItem(parentItem, File);
				path = xml.attributes().value("path").toString();
			} else if( !name.compare("dependence", Qt::CaseSensitive) ) {
				newItem = new CWorkSpaceItem(parentItem, DependenceFolder);
			} else if( !name.compare("pack", Qt::CaseSensitive) ) {
				newItem = new CWorkSpaceItem(parentItem, PackFolder);
			} else if( !name.compare("source", Qt::CaseSensitive) ) {
				newItem = new CWorkSpaceItem(parentItem, SourceFolder);
			} else if( !name.compare("resource", Qt::CaseSensitive) ) {
				newItem = new CWorkSpaceItem(parentItem, ResourceFolder);
			} else if( !name.compare("hspide", Qt::CaseSensitive) ) {
				continue;
			} else {
				delete newItem;
				xml.clear();
				return false;
			}
			// UUID�����݂��鎞�̂�UUID�Z�b�g
			if( !path.isEmpty() ) {
				QUuid uuid(xml.attributes().value("uuid").toString());
				if( !uuid.isNull() ) {
					theFile.assign(path, uuid);
				} else {
					uuid = theFile.assign(path);
				}
				newItem->m_uuid = uuid;
			}
			// �e�L�X�g���w��
			switch( newItem->m_type )
			{
			case File:
			case Folder:
			case Project:
				newItem->setText(theFile.fileName(newItem->m_uuid));
				break;
			case Solution: {
				QString name = xml.attributes().value("name").toString();
				if( !name.isEmpty() ) {
					newItem->setText(name);
				} else if( theFile.isUntitled(newItem->m_uuid) ) {
					newItem->setText( theFile.fileName(newItem->m_uuid) );
				} else {
					newItem->setText( theFile.fileInfo(newItem->m_uuid).baseName() );
				}
				break; }
			default:
				;
			}
			if( newItem != this )
			{
				// �c���[���f�����Z�b�g
				newItem->setModel(m_model);
				if( m_model ) {
					m_model->appendRow(newItem, parentItem->index());
					m_model->setData(newItem->index(), newItem->text(), Qt::DisplayRole);
				} else {
					parentItem->insert(parentItem->count(), newItem);
				}
				vItems.push_back(newItem);
			}
			// �ʒm
			if( !newItem->m_uuid.isNull() ) {
				onFileChanged(newItem->m_uuid);
			}
			break; }
		case QXmlStreamReader::EndElement: {
			QString name = xml.name().toString();
			if( name.compare("solution", Qt::CaseSensitive) &&
				name.compare("hspide", Qt::CaseSensitive) )
			{
				vItems.pop_back();
			}
			break; }
		}
	}

	// �G���[�`�F�b�N������
	if( xml.hasError() )
	{
		xml.clear();
		return false;
	}

	xml.clear();

	return false;
}

bool CWorkSpaceItem::serialize(QXmlStreamWriter * xml)
{
	bool uuidValid = false;

	switch( m_type )
	{
	case File:
		xml->writeStartElement("file");
		xml->writeAttribute("path", m_path);
		uuidValid = true;
		break;
	case Folder:
		xml->writeStartElement("folder");
		xml->writeAttribute("name", m_text);
		break;
	case Solution:
		xml->writeStartElement("solution");
		xml->writeAttribute("name", m_text);
		uuidValid = true;
		break;
	case Project:
		xml->writeStartElement("project");
		xml->writeAttribute("path", m_path);
		uuidValid = true;
		break;
	case DependenceFolder:
		xml->writeStartElement("dependence");
		break;
	case PackFolder:
		xml->writeStartElement("pack");
		break;
	case SourceFolder:
		xml->writeStartElement("source");
		break;
	case ResourceFolder:
		xml->writeStartElement("resource");
		break;
	default:
		return false;
	}

	if( uuidValid ) {
		xml->writeAttribute("uuid", m_uuid.toString());
	}

	// �q���S�ăV���A���C�Y
	foreach(CWorkSpaceItem* item, m_children)
	{
		item->serialize(xml);
	}

	xml->writeEndElement();

	return true;
}

bool CWorkSpaceItem::deserialize(QXmlStreamReader * xml)
{
	return false;
}

// �A�C�e���Ɗ֘A�t��
bool CWorkSpaceItem::setAssignDocument(CDocumentPane * doc)
{
	m_assignDocument = doc;
	return true;
}

CDocumentPane * CWorkSpaceItem::assignDocument()
{
	return m_assignDocument;
}

void CWorkSpaceItem::onFileChanged(const QUuid& uuid)
{
	// ���f���ɍX�V��ʒm���r���[���ĕ`��
	if( uuid == m_uuid &&
		m_model )
	{
		m_model->dataChanged(index(), index());
	}
}
