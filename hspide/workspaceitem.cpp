#include "workspacemodel.h"
#include "workspaceitem.h"
#include "documentpane.h"
#include <QApplication>
#include <QFileInfo>
#include <QFileDialog>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

CWorkSpaceItem::CWorkSpaceItem(QObject * parent, CWorkSpaceModel * model)
	: QObject(parent)
	, m_model(model)
	, m_parent(NULL)
	, m_parentPos(0)
	, m_type(UnkownType)
	, m_nodeType(UnkownNodeType)
	, m_assignDocument(NULL)
	, m_uuid(QUuid::createUuid())
{
	m_icon = QIcon(":/images/tango/small/text-x-generic.png");
	m_text = QString("this=%1").arg((int)this, 0, 16);
}

CWorkSpaceItem::CWorkSpaceItem(QObject * parent, Type type, CWorkSpaceModel * model)
	: QObject(parent)
	, m_model(NULL)
	, m_parent(NULL)
	, m_parentPos(0)
	, m_assignDocument(NULL)
	, m_uuid(QUuid::createUuid())
{
	setType(type);

	// 種別に応じて属性をセット
	switch( m_type )
	{
	case File:
		setIcon(QIcon(":/images/tango/small/text-x-generic.png"));
		break;
	case Folder:
		setIcon(QIcon(":/images/tango/small/folder.png"));
		break;
	case Project:
		setIcon(QIcon(":/images/tango/small/folder.png"));
		setText();
		break;
	case Solution:
		setIcon(QIcon(":/images/tango/small/edit-copy.png"));
		setText(tr("(untitled)"));
		break;
	case DependenceFolder:
		setIcon(QIcon(":/images/tango/small/folder.png"));
		setText(tr("Dependence"));
		break;
	case PackFolder:
		setIcon(QIcon(":/images/tango/small/folder.png"));
		setText(tr("Packfile"));
		break;
	case SourceFolder:
		setIcon(QIcon(":/images/tango/small/folder.png"));
		setText(tr("Source"));
		break;
	case ResourceFolder:
		setIcon(QIcon(":/images/tango/small/folder.png"));
		setText(tr("Resource"));
		break;
	default:
		;
	}
	
	m_model = model;
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

	switch( m_type )
	{
	case File:
	case Project:
		m_nodeType = FileNode;
		break;
	case Folder:
	case Solution:
	case DependenceFolder:
	case PackFolder:
	case SourceFolder:
	case ResourceFolder:
		m_nodeType = FolderNode;
		break;
	default:
		;
	}
}

CWorkSpaceItem::NodeType CWorkSpaceItem::nodeType() const
{
	return m_nodeType;
}

void CWorkSpaceItem::setNodeType(NodeType type)
{
	m_nodeType = type;
}

const QString & CWorkSpaceItem::path() const
{
	return m_path;
}

void CWorkSpaceItem::setPath(const QString & path)
{
	m_path = path;

	switch( m_type )
	{
	case File:
	case Folder:
	case Project:
		setText(path.isEmpty() ? QString() : QFileInfo(path).fileName());
		break;
	case Solution:
		setText(path.isEmpty() ? QString() : QFileInfo(path).baseName());
		break;
	default:
		;
	}
}

const QString & CWorkSpaceItem::text() const
{
	return m_text;
}

void CWorkSpaceItem::setText(const QString & text)
{
	m_text = text.isEmpty() ? tr("(untitled)") : text;
	// モデルに更新を通知しビューを再描画
	if( m_model ) {
		m_model->setData(index(), m_text, Qt::DisplayRole);
	}
}

const QIcon & CWorkSpaceItem::icon() const
{
	return m_icon;
}

void CWorkSpaceItem::setIcon(const QIcon & icon)
{
	m_icon = icon;
	// モデルに更新を通知しビューを再描画
	if( m_model ) {
		m_model->setData(index(), m_icon, Qt::DecorationRole);
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

bool CWorkSpaceItem::isUntitled() const
{
	if( m_assignDocument )
	{
		return m_assignDocument->isUntitled();
	}
	else
	{
		switch(m_type)
		{
		case Solution:
			return m_text.isEmpty() || m_text == tr("(untitled)");
		case Project:
		case File:
			return QFileInfo(m_path).baseName().isEmpty();
		}
	}
	return true;
}

const QUuid & CWorkSpaceItem::uuid() const
{
	return m_uuid;
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

	// 子のブレイクポイントも列挙
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
	// 子も全て保存
	foreach(CWorkSpaceItem* item, m_children)
	{
		item->save();
	}

	if( m_assignDocument )
	{
		if( m_assignDocument->save(fileName) )
		{
			setPath(m_assignDocument->filePath());
		}
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

	// ファイル名が指定されていない場合はダイアログを表示して表示
	if( filePath.isEmpty() )
	{
		filePath = QFileDialog::getSaveFileName(QApplication::activeWindow(),
						tr("Save File"), tr("untitled") + ".hspsln",
						tr("HSP IDE Solution File (*.hspsln)") + ";;" +
						tr("All File (*.*)")
						);
	}

	// 書き込み用のファイルをオープン
	QFile file(filePath);
	if( !file.open(QFile::WriteOnly | QFile::Text) )
	{
		return false;
	}

	setPath(filePath);

	// ファイルにXMLとして出力
	xml.setDevice(&file);
	xml.writeStartDocument();
	xml.writeDTD("<!DOCTYPE xbel>");
	xml.writeStartElement("hspide");
	xml.writeAttribute("version", "1.0");
	xml.writeAttribute("type", "solution");

	// アイテムツリーをXMLに変換
	serialize(&xml);

	xml.writeEndDocument();

	return true;
}

bool CWorkSpaceItem::loadSolution(const QString & fileName)
{
	// 読み込み用のファイルをオープン
	QFile file(fileName);
	if( !file.open(QFile::ReadOnly | QFile::Text) )
	{
		return false;
	}

	QXmlStreamReader xml(&file);
	QVector<CWorkSpaceItem*> vItems;

	// XMLを終わりまで読み込む
	while( !xml.atEnd() &&
	       !xml.hasError() )
	{
		QXmlStreamReader::TokenType token = xml.readNext();

		switch( token )
		{
		case QXmlStreamReader::StartDocument:
			// 種別をチェック
			if( !xml.attributes().value("type").compare("solution", Qt::CaseSensitive) )
			{
				xml.clear();
				return false;
			}
			// ルートを指定
			vItems.push_back(this);
			if( m_model ) {
				m_model->setData(index(), m_text, Qt::DisplayRole);
			}
			break;
		case QXmlStreamReader::StartElement: {
			CWorkSpaceItem* parentItem = vItems.back();
			CWorkSpaceItem* newItem    = NULL;
			QString name = xml.name().toString();
			       if( !name.compare("solution", Qt::CaseSensitive) ) {
				if( 1 != vItems.size() ) {
					xml.clear();
					return false;
				}
				setText(xml.attributes().value("name").toString());
				continue;
			} else if( !name.compare("project", Qt::CaseSensitive) ) {
				newItem = new CWorkSpaceItem(parentItem, Project);
				newItem->setPath(xml.attributes().value("path").toString());
			} else if( !name.compare("folder", Qt::CaseSensitive) ) {
				newItem = new CWorkSpaceItem(parentItem, Folder);
				newItem->setText(xml.attributes().value("name").toString());
			} else if( !name.compare("file", Qt::CaseSensitive) ) {
				newItem = new CWorkSpaceItem(parentItem, File);
				newItem->setPath(xml.attributes().value("path").toString());
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
			// UUIDが存在する時のみUUIDセット
			QUuid uuid(xml.attributes().value("uuid").toString());
			if( !uuid.isNull() ) {
				newItem->m_uuid = uuid;
			}
			// ツリーモデルをセット
			newItem->setModel(m_model);
			if( m_model ) {
				m_model->appendRow(newItem, parentItem->index());
				m_model->setData(newItem->index(), newItem->text(), Qt::DisplayRole);
			} else {
				parentItem->insert(parentItem->count(), newItem);
			}
			vItems.push_back(newItem);
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

	// エラーチェック＆処理
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

	switch( m_type )
	{
	case File:
		xml->writeStartElement("file");
		xml->writeAttribute("path", m_path);
		break;
	case Folder:
		xml->writeStartElement("folder");
		xml->writeAttribute("name", m_text);
		break;
	case Solution:
		xml->writeStartElement("solution");
		xml->writeAttribute("name", m_text);
		break;
	case Project:
		xml->writeStartElement("project");
		xml->writeAttribute("path", m_path);
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

	xml->writeAttribute("uuid", m_uuid.toString());

	// 子も全てシリアライズ
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

// アイテムと関連付け
bool CWorkSpaceItem::setAssignDocument(CDocumentPane * doc)
{
	m_assignDocument = doc;
	return true;
}

CDocumentPane * CWorkSpaceItem::assignDocument()
{
	return m_assignDocument;
}
