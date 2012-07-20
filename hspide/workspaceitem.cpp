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

CWorkSpaceItem::CWorkSpaceItem(QObject* parent, CWorkSpaceModel* model)
	: QObject(parent)
	, m_model(model)
	, m_parent(NULL)
	, m_parentPos(0)
	, m_type(UnkownType)
	, m_nodeType(UnkownNodeType)
	, m_assignDocument(NULL)
	, m_buildConfigurations(theConf.buildConf())
{
	m_icon = QIcon(":/images/tango/small/text-x-generic.png");
	m_text = QString("this=%1").arg((int)this, 0, 16);

	// 新しいUUIDを割り当て
	for(Configuration::BuildConfList::iterator
			ite = m_buildConfigurations.begin(),
			last= m_buildConfigurations.end();
		ite != last; ++ite)
	{
		ite->uuid = QUuid::createUuid();
	}

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

	// 通知を登録
	connect(&theFile, SIGNAL(filePathChanged(QUuid)), this, SLOT(onFileChanged(QUuid)));
}

CWorkSpaceItem::CWorkSpaceItem(QObject* parent, Type type, CWorkSpaceModel* model)
	: QObject(parent)
	, m_model(NULL)
	, m_parent(NULL)
	, m_parentPos(0)
	, m_type(type)
	, m_assignDocument(NULL)
	, m_buildConfigurations(theConf.buildConf())
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

	// 新しいUUIDを割り当て
	for(Configuration::BuildConfList::iterator
			ite = m_buildConfigurations.begin(),
			last= m_buildConfigurations.end();
		ite != last; ++ite)
	{
		ite->uuid = QUuid::createUuid();
	}

	// 種別に応じて属性をセット
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

	// 通知を登録
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

	// モデルに更新を通知しビューを再描画
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

	// モデルに更新を通知しビューを再描画
	if( m_model ) {
		m_model->dataChanged(index(), index());
	}
}

// ファイルパスを取得
QString CWorkSpaceItem::path() const
{
	return theFile.path(m_uuid);
}

const QString& CWorkSpaceItem::text() const
{
	return m_text;
}

void CWorkSpaceItem::setText(const QString& text)
{
	m_text = text;

	// モデルに更新を通知しビューを再描画
	if( m_model ) {
		m_model->dataChanged(index(), index());
	}
}

const QIcon& CWorkSpaceItem::icon() const
{
	return m_icon;
}

void CWorkSpaceItem::setIcon(const QIcon& icon)
{
	m_icon = icon;

	// モデルに更新を通知しビューを再描画
	if( m_model ) {
		m_model->dataChanged(index(), index());
	}
}

const QStringList& CWorkSpaceItem::suffixFilter() const
{
	return m_suffixFilters;
}

void CWorkSpaceItem::setSuffixFilter(const QStringList& filters)
{
	m_suffixFilters = filters;

	// モデルに更新を通知しビューを再描画
	if( m_model ) {
		m_model->dataChanged(index(), index());
	}
}

int CWorkSpaceItem::count() const
{
	return m_children.size();
}

CWorkSpaceItem* CWorkSpaceItem::at(int index) const
{
	return
		0 <= index && index < m_children.size()
			? m_children[index]
			: NULL;
}

CWorkSpaceItem* CWorkSpaceItem::parent() const
{
	return m_parent;
}

CWorkSpaceItem* CWorkSpaceItem::ancestor(Type type)
{
	return m_parent ? type == m_type ? this
	                                 : m_parent->ancestor(type)
	                : NULL;
}

CWorkSpaceItem* CWorkSpaceItem::search(const QString& path, bool basename)
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

CWorkSpaceItem* CWorkSpaceItem::search(const QUuid& uuid)
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

bool CWorkSpaceItem::insert(int position, CWorkSpaceItem* item)
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

// 無題ファイル(ディスクに保存していないファイル)か？
bool CWorkSpaceItem::isUntitled() const
{
	return theFile.isUntitled(m_uuid);
}

const QUuid& CWorkSpaceItem::uuid() const
{
	return m_uuid;
}

void CWorkSpaceItem::setUuid(const QUuid& uuid)
{
	m_uuid = uuid;
}

const Configuration::BuildConfList& CWorkSpaceItem::buildConf() const
{
	return m_buildConfigurations;
}

void CWorkSpaceItem::setBuildConf(const Configuration::BuildConfList& conf)
{
	m_buildConfigurations = conf;
}

const QUuid& CWorkSpaceItem::buildTarget() const
{
	return m_buildTarget;
}

void CWorkSpaceItem::setBuildTarget(const QUuid& target)
{
	m_buildTarget = target;
}

bool CWorkSpaceItem::load(const QString& fileName)
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

bool CWorkSpaceItem::save(SaveType saveType, bool noReclusive)
{
	return save(QString(), saveType, noReclusive);
}

bool CWorkSpaceItem::save(const QString& fileName, SaveType saveType, bool noReclusive)
{
	if( !noReclusive )
	{
		// 子も全て保存
		foreach(CWorkSpaceItem* item, m_children)
		{
			if( item->assignDocument() &&
				item->assignDocument()->isModified() )
			{
				item->save(saveType);
			}
		}
	}

	if( m_assignDocument )
	{
		m_assignDocument->save(fileName, SaveAs == saveType);
	}
	else
	{
		switch(m_type)
		{
		case Solution:
			saveSolution(fileName, SaveAs == saveType);
			break;
		}
	}

	return true;
}

bool CWorkSpaceItem::saveSolution(const QString& fileName, bool saveAs)
{
	QXmlStreamWriter xml;
	QString filePath = fileName;

	// ファイル名が指定されていない場合はダイアログを表示して表示
	if( filePath.isEmpty() || saveAs )
	{
		QString defaultName = tr("untitled") + ".hspsln";
		if( !filePath.isEmpty() && !saveAs ) {
			QFileInfo fileInfo(filePath);
			defaultName
				= fileInfo.dir()
					.absoluteFilePath(fileInfo.baseName() + ".hspsln");
			defaultName = QDir::toNativeSeparators(defaultName);
		}

		filePath = QFileDialog::getSaveFileName(QApplication::activeWindow(),
						tr("Save File"), defaultName,
						tr("HSP IDE Solution File (*.hspsln)") + ";;" +
						tr("All File (*.*)")
						);
		if( filePath.isEmpty() )
		{
			return false;
		}
	}

	// 書き込み用のファイルをオープン
	QFile file(filePath);
	if( !file.open(QFile::WriteOnly | QFile::Text) )
	{
		return false;
	}

	theFile.rename(m_uuid, filePath);

	// ファイルにXMLとして出力
	xml.setDevice(&file);
	xml.writeStartDocument();
//	xml.writeDTD("<!DOCTYPE hspide>");
	xml.writeStartElement("hspide");
	xml.writeAttribute("version", "1.0");
	xml.writeAttribute("type", "solution");

	// アイテムツリーをXMLに変換
	serialize(&xml);

	xml.writeEndDocument();

	return true;
}

bool CWorkSpaceItem::loadSolution(const QString& fileName)
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
			} else if( !name.compare("build", Qt::CaseSensitive) ) {
				continue;
			} else if( !name.compare("configuration", Qt::CaseSensitive) ) {
				Configuration::BuildConfList buildConf = parentItem->buildConf();
				Configuration::BuildConfType conf;
				conf.name           = xml.attributes().value("name").toString();
				conf.uuid           = QUuid(xml.attributes().value("uuid").toString());
				conf.preprocessOnly = 0 != xml.attributes().value("preprocess_only").toString().toInt();
				conf.compile        = 0 != xml.attributes().value("compile").toString().toInt();
				conf.make           = 0 != xml.attributes().value("make").toString().toInt();
				conf.noExecute      = 0 != xml.attributes().value("no_execute").toString().toInt();
				conf.debug          = 0 != xml.attributes().value("debug").toString().toInt();
				if( !conf.uuid.isNull() ) {
					buildConf.push_back(conf);
					parentItem->setBuildConf(buildConf);
				}
				continue;
			} else {
				delete newItem;
				xml.clear();
				return false;
			}
			// UUIDが存在する時のみUUIDセット
			if( !path.isEmpty() ) {
				QUuid uuid(xml.attributes().value("uuid").toString());
				if( !uuid.isNull() ) {
					theFile.assign(path, uuid);
				} else {
					uuid = theFile.assign(path);
				}
				newItem->m_uuid = uuid;
			}
			// テキストを指定
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
			{
				QUuid uuid(xml.attributes().value("build").toString());
				if( !uuid.isNull() ) {
					newItem->setBuildTarget(uuid);
				}
			}
			if( newItem != this )
			{
				// ツリーモデルをセット
				newItem->setModel(m_model);
				if( m_model ) {
					m_model->appendRow(newItem, parentItem->index());
					m_model->setData(newItem->index(), newItem->text(), Qt::DisplayRole);
				} else {
					parentItem->insert(parentItem->count(), newItem);
				}
				vItems.push_back(newItem);
			}
			// 通知
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

	// エラーチェック＆処理
	if( xml.hasError() )
	{
		xml.clear();
		return false;
	}

	xml.clear();

	return false;
}

bool CWorkSpaceItem::serialize(QXmlStreamWriter* xml)
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
		//
		xml->writeStartElement("build");
		foreach(Configuration::BuildConfType buildConf, m_buildConfigurations)
		{
			xml->writeStartElement("configuration");
			xml->writeAttribute("name",				buildConf.name);
			xml->writeAttribute("uuid",				buildConf.uuid.toString());
			xml->writeAttribute("preprocess_only",	buildConf.preprocessOnly	? "true" : "false");
			xml->writeAttribute("compile",			buildConf.compile			? "true" : "false");
			xml->writeAttribute("make",				buildConf.make				? "true" : "false");
			xml->writeAttribute("no_execute",		buildConf.noExecute			? "true" : "false");
			xml->writeAttribute("debug",			buildConf.debug				? "true" : "false");
			xml->writeEndElement();
		}
		xml->writeEndElement();
		//
		xml->writeAttribute("build", m_buildTarget.toString());
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

	// 子も全てシリアライズ
	foreach(CWorkSpaceItem* item, m_children)
	{
		item->serialize(xml);
	}

	xml->writeEndElement();

	return true;
}

bool CWorkSpaceItem::deserialize(QXmlStreamReader* xml)
{
	return false;
}

// アイテムと関連付け
bool CWorkSpaceItem::setAssignDocument(CDocumentPane* doc)
{
	m_assignDocument = doc;
	return true;
}

CDocumentPane* CWorkSpaceItem::assignDocument()
{
	return m_assignDocument;
}

void CWorkSpaceItem::onFileChanged(const QUuid& uuid)
{
	// モデルに更新を通知しビューを再描画
	if( uuid == m_uuid &&
		m_model )
	{
		m_model->dataChanged(index(), index());
	}
}
