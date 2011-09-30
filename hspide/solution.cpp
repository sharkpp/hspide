#include <QCryptographicHash>
#include <QtDebug>
#include "solution.h"
#include "project.h"
#include "projectitem.h"
#include "editor.h"

CSolution::CSolution(QObject *parent, const CSolution::Config & config)
	: QObject(parent)
	, CProjectItem(parent)
	, mConfig(config)
{
}

CSolution::~CSolution()
{
}

//int CSolution::columnCount(const QModelIndex & parent ) const
//{
//	return 1;
//}
//
//QVariant CSolution::data(const QModelIndex & index, int role ) const
//{
//	if( !index.isValid() ||
//		role != Qt::DisplayRole )
//	{
//		return QVariant();
//	}
//	return QVariant();
//}
//
//Qt::ItemFlags CSolution::flags( const QModelIndex & index ) const
//{
//	if( !index.isValid() ) {
//		return 0;
//	}
//	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
//}
//
//QVariant CSolution::headerData( int section, Qt::Orientation orientation, int role ) const
//{
//	return QVariant();
//}
//
//QModelIndex CSolution::index( int row, int column, const QModelIndex & parent ) const
//{
//	if( !hasIndex(row, column, parent) )
//	{
//		return QModelIndex();
//	}
//
//	QObject *parent, *child = NULL;
//
//	if( !parent.isValid() ) {
//		parent = static_cast<QObject*>(this);
//	} else {
//		QObject *target;
//		target = static_cast<QObject*>(parent.internalPointer());
//		parent = static_cast<QObject*>(target->parent());
//	}
//	CSolution   * solution = dynamic_cast<CSolution*>(parent);
//	CProject    * project  = dynamic_cast<CProject*>(parent);
//	CProjectItem* item     = dynamic_cast<CProjectItem*>(parent);
//
//	if( solution ) {
//		if( NULL != (child = solution->child(row)) ) {
//			return createIndex(row, column, child);
//		}
//	} else if( project ) {
//		if( NULL != (child = project->child(row)) ) {
//			return createIndex(row, column, child);
//		}
//	} else if( item ) {
//		if( NULL != (child = item->child(row)) ) {
//			return createIndex(row, column, child);
//		}
//	}
//
//	return QModelIndex();
//}
//
//QModelIndex CSolution::parent( const QModelIndex & index ) const
//{
//	if( !index.isValid() ||
//		this == index.internalPointer() )
//	{
//		return QModelIndex();
//	}
//
//	// ポインタからダイナミックキャストでソリューション、プロジェクト、フォルダ
//	// に変換テスト
//	QObject* target = static_cast<QObject*>(index.internalPointer());
////	CSolution   * solution = dynamic_cast<CSolution*>(target); // ありえない
//	CProject    * project  = dynamic_cast<CProject*>(target);
//	CProjectItem* item     = dynamic_cast<CProjectItem*>(target);
//
//	if( project ) {
//		return createIndex(getRow(project), 0, project);
//	}
//
//	if( item ) {
//		return createIndex(dynamic_cast<CProject*>(item->parent())->getRow(item), 0, item);
//	}
//
//	return QModelIndex();
//}
//
//int CSolution::rowCount( const QModelIndex & parent ) const
//{
//	if( !parent.valid() ) {
//		return mProjects.size();
//	}
//
//	// ポインタからダイナミックキャストでソリューション、プロジェクト、フォルダ
//	// に変換テスト
//	QObject* obj = static_cast<QObject*>(parent.internalPointer());
////	CSolution   * solution = dynamic_cast<CSolution*>(obj); // ありえない
//	CProject    * project  = dynamic_cast<CProject*>(obj);
//	CProjectItem* item     = dynamic_cast<CProjectItem*>(obj);
//
//	if( project ) {
//		return project->childCount();
//	}
//
//	if( item ) {
//		return item->childCount();
//	}
//
//	return 0;
//}
//
//int CSolution::getRow(const CProject & project) const
//{
//	int row = mProjects.size();
//	for(QMap<QString, QSharedPointer<CProject> >::iterator
//			ite = mProjects.begin(), last= mProjects.end();
//		ite != last; ++ite, ++row)
//	{
//		if( (*ite).data() == &project ) {
//			break;
//		}
//	}
//	if( mProjects.size() <= row ) {
//		return 0;
//	}
//	return row;
//}
//
//CProject * CSolution::child(int row)
//{
//	if( mProjects.size() <= row ) {
//		return 0;
//	}
//	return mProjects[row].data();
//}

// ソリューションを読み込み
bool CSolution::load(const QString & filename)
{
	QXmlSimpleReader reader;
	reader.setContentHandler(this);
	reader.setErrorHandler(this);

	QFile file(filename);
	if( !file.open(QFile::ReadOnly | QFile::Text) ) {
		return false;
	}

	QXmlInputSource xmlInputSource(&file);
	if( !reader.parse(xmlInputSource) ) {
		return false;
	}

	return true;
}

// ソリューションを保存
bool CSolution::save(const QString & filename)
{
	return false;
}

// ソリューションにプロジェクトを追加
QSharedPointer<CProject> & CSolution::append(const QString & filename)
{
	CProject::Config config;
//	config.compilerPath = ""

	QSharedPointer<CProject> newProject(new CProject(this, config));
	mProjects[filename] = newProject;

	// シグナルの通知先を登録
	connect(&*newProject, SIGNAL(buildStart()),                this, SLOT(buildStartProject()));
	connect(&*newProject, SIGNAL(buildFinished(bool)),         this, SLOT(buildFinishedProject(bool)));
	connect(&*newProject, SIGNAL(buildOutput(const QString&)), this, SLOT(buildOutputProject(const QString&)));

	return newProject;
}

// ソリューションからプロジェクトを除外
bool CSolution::remove(const QString & filename)
{
	QSharedPointer<CProject> oldProject = mProjects[filename];

	// シグナルの通知先の登録解除
//	disconnect(newProject, );

	mProjects.remove(filename);

	return false;
}

// ソリューションへエディタを関連付け
bool CSolution::openFile(CEditor* editor)
{
	for(QMap<QString, QSharedPointer<CProject> >::iterator
			ite = mProjects.begin(),
			last= mProjects.end();
		ite != last; ++ite)
	{
		(*ite)->openFile(editor);
	}
	return true;
}

// ソリューションからエディタを解除
bool CSolution::closeFile(CEditor* editor)
{
	for(QMap<QString, QSharedPointer<CProject> >::iterator
			ite = mProjects.begin(),
			last= mProjects.end();
		ite != last; ++ite)
	{
		(*ite)->closeFile(editor);
	}
	return true;
}

/*
// ソリューションのビルド
void CSolution::build()
{
	for(QMap<QString, QSharedPointer<CProject> >::iterator
			ite = mProjects.begin(),
			last= mProjects.end();
		ite != last; ++ite)
	{
		// ビルド
		(*ite)->build();
	}

	emit buildStart();
}

// プロジェクトのビルド開始
void CSolution::buildStartProject()
{
}

// プロジェクトのビルド完了
void CSolution::buildFinishedProject(bool successed)
{
	emit buildFinished(successed);
}

// ビルド中の出力を取得
void CSolution::buildOutputProject(const QString & output)
{
	emit buildOutput(output);
}

// ソリューションの実行
void CSolution::run()
{
}
*/

bool CSolution::startElement(const QString &namespaceURI, const QString &localName,
                             const QString &qName, const QXmlAttributes &attributes)
{
	qDebug() << __FUNCTION__ << "(\"" << namespaceURI << "\",\"" << localName << "\",\"" << qName << "\",\"" << "\")";
	return true;
}

bool CSolution::endElement(const QString &namespaceURI, const QString &localName,
                           const QString &qName)
{
	qDebug() << __FUNCTION__ << "(\"" << namespaceURI << "\",\"" << localName << "\",\"" << qName << "\")";
	return true;
}

bool CSolution::characters(const QString &str)
{
	qDebug() << __FUNCTION__ << "(\"" << str << "\")";
	return true;
}

bool CSolution::fatalError(const QXmlParseException &exception)
{
	return true;
}
