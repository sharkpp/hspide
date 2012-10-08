#include <QDebug>
#include <QDir>
#include <QLocalSocket>
#include <QTemporaryFile>
#include "compiler.h"
#include "workspaceitem.h"
#include "documentpane.h"
#include "debugger.h"

CCompiler::CCompiler(QObject *parent)
	: QObject(parent)
	, m_uuid(QUuid::createUuid())
	, m_hspCompPath(QDir::toNativeSeparators("./"))
	, m_hspPath(QDir::toNativeSeparators("./"))
	, m_hspCommonPath(QDir::toNativeSeparators("./common/"))
	, m_targetItem(NULL)
	, m_compiler(NULL)
	, m_objTemp(NULL)
{
	updateCompilerPath();

	// 設定の変更通史の登録と設定からの初期化処理
	connect(theConf, SIGNAL(updateConfiguration(const Configuration*)),
	        this,  SLOT(updateConfiguration(const Configuration*)));
	updateConfiguration(theConf);

	m_hspErrorText
		<< ""									// 0
		<< tr("Unknown system error")			// 1
		<< tr("Syntax error")					// 2
		<< tr("Illegal function call")			// 3
		<< tr("Wrong expression")				// 4
		<< tr("Default parameter not allowed")	// 5
		<< tr("Type mismatch")					// 6
		<< tr("Array overflow")					// 7
		<< tr("Not a label object")				// 8
		<< tr("Too many nesting")				// 9
		<< tr("Return without gosub")			// 10
		<< tr("Loop without repeat")			// 11
		<< tr("File I/O error")					// 12
		<< tr("Picture file missing")			// 13
		<< tr("External execute error")			// 14
		<< tr("Priority error")					// 15
		<< tr("Too many parameters")			// 16
		<< tr("Temporary buffer overflow")		// 17
		<< tr("Wrong valiable name")			// 18
		<< tr("Divided by zero")				// 19
		<< tr("Buffer Overflow")				// 20
		<< tr("Unsupported function")			// 21
		<< tr("Expression too complex")			// 22
		<< tr("Variable required")				// 23
		<< tr("Integer value required")			// 24
		<< tr("Bad array expression")			// 25
		<< tr("Out of memory")					// 26
		<< tr("Type initalization failed")		// 27
		<< tr("No Function parameters")			// 28
		<< tr("Stack overflow")					// 29
		<< tr("Invalid parameter name")			// 30
		<< tr("Invalid type of array")			// 31
		<< tr("Invalid function parameter")		// 32
		<< tr("Too many window objects")		// 33
		<< tr("Invalid Array/Function")			// 34
		<< tr("Structure required")				// 35
		<< tr("Illegal structure call")			// 36
		<< tr("Invalid type of variable")		// 37
		<< tr("DLL call failed")				// 38
		<< tr("External COM call failed")		// 39
		<< tr("No function result")				// 40
		<< tr("Invalid syntax for function")	// 41
		;

}

const QUuid& CCompiler::uuid() const
{
	return m_uuid;
}

QTemporaryFile*	CCompiler::objTemp()
{
	return m_objTemp;
}

// シンボル一覧の取得
bool CCompiler::collectSymbols()
{
	QString program = QDir(m_hspCompPath).absoluteFilePath("hspcmp.exe");

	if( !QFileInfo(program).exists() )
	{
		return false;
	}

	QStringList arguments;
	arguments << "--symbol-put";

	QProcess* process = new QProcess(this);

	// 処理完了時の通知を登録
	connect(process, SIGNAL(finished(int,QProcess::ExitStatus)),
			this, SLOT(listedSymbolsFinished(int,QProcess::ExitStatus)));

qDebug() << program << arguments;
	process->start(program, arguments);

	if( !process->waitForStarted() )
	{
		delete process;
		return false;
	}
	return true;
}

// シンボル一覧を取得
const CSymbolsList& CCompiler::symbols() const
{
	return m_highlightSymbols;
}

// エラー文字列を取得
const QStringList& CCompiler::hspErrorTexts() const
{
	return m_hspErrorText;
}

CWorkSpaceItem* CCompiler::compileItem()
{
	return m_targetItem;
}

// コンパイラ呼び出し
bool CCompiler::compile(CWorkSpaceItem* targetItem)
{
	QString filename = targetItem->path();
	QUuid   uuid     = targetItem->uuid();

	Configuration::BuildConfType
			buildConf = targetItem->currentBuildConf();

	m_targetItem = targetItem;
	m_objTemp = NULL;

	// シグナル発報
	emit compileStarted(uuid);

	switch( targetItem->type() )
	{
	case CWorkSpaceItem::Project:
		break;
	default:
		emit compileFailure();
		return false;
	}

	bool tempSave = false;

	QProcess* hspcmp = new QProcess(this);

	// 作業ディレクトリを指定
#ifdef _WIN32
	// My Documents の場所を取得
    QSettings settings(QSettings::UserScope, "Microsoft", "Windows");
    settings.beginGroup("CurrentVersion/Explorer/Shell Folders");
    QString workDir = settings.value("Personal").toString();
#else
	QString workDir = QDir::homePath();
#endif

	if( !targetItem->isUntitled() )
	{
		workDir = QFileInfo(filename).dir().path();
	}

	// 一時的なファイルに保存
	{
		CDocumentPane* document = targetItem->assignDocument();

		if( !document )
		{
			emit compileFailure();
			delete hspcmp;
			return false;
		}

		// 一時的なファイルに保存
		tempSave = true;
		QString baseName = QDir::toNativeSeparators(QDir(workDir).absoluteFilePath("hsptmp"));
		QTemporaryFile* tempFile = new QTemporaryFile(baseName, hspcmp);
		tempFile->setAutoRemove(true);
		if( !tempFile->open() )
		{
			emit compileFailure();
			delete hspcmp;
			return false;
		}
		filename = tempFile->fileName();
		document->save(filename, false, true);
	}

	QString program = QDir::toNativeSeparators(QDir(m_hspCompPath).absoluteFilePath("hspcmp.exe"));
	QStringList arguments;
	arguments << "-C" << m_hspCommonPath
	          << "-H" << m_hspPath
			  ;
	if( buildConf.debug ) { // デバッグモード
		arguments << "-d"
			      << "--hspide-fix" // HSPIDE用補正
				  ;
	} else if( buildConf.make ) {
		arguments << "-m";
	}
	if( buildConf.preprocessOnly ) { // デバッグモード
		arguments << "-P";
	}
	if( /*tempSave*/buildConf.debug ) { // 一時的なファイルに保存
		QString baseName = QDir::toNativeSeparators(QDir(workDir).absoluteFilePath("obj"));
		m_objTemp = new QTemporaryFile(baseName, parent());
		m_objTemp->setAutoRemove(true);
		m_objTemp->open();
		arguments << "-o" << m_objTemp->fileName()
		          << "-r" << QString("?%1").arg(targetItem->uuid().toString());
	} else {
		arguments << "-r" << QString("?%1").arg(targetItem->uuid().toString());
	}
	arguments << filename;
	if( !buildConf.debug ) { 
		arguments << "start.ax";
	}

	// 処理完了時の通知を登録
	connect(hspcmp, SIGNAL(error(QProcess::ProcessError)),      this, SLOT(hspcmpError(QProcess::ProcessError)));
	connect(hspcmp, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(hspcmpFinished(int,QProcess::ExitStatus)));
	connect(hspcmp, SIGNAL(readyReadStandardOutput()),          this, SLOT(hspcmpReadOutput()));

	m_compiler   = hspcmp;

qDebug() << program << arguments;
	hspcmp->setWorkingDirectory(workDir);
	hspcmp->start(program, arguments);

	if( !hspcmp->waitForStarted() )
	{
		delete m_objTemp;
		delete hspcmp;
		emit compileFailure();
		m_compiler   = NULL;
		m_objTemp    = NULL;
		return false;
	}

	return true;
}

void CCompiler::updateCompilerPath()
{
}

void CCompiler::updateConfiguration(const Configuration* conf)
{
	bool update = false;
	QString hspPath       = QDir::toNativeSeparators(conf->hspPath());
	QString hspCompPath   = QDir::toNativeSeparators(conf->compilerPath());
	QString hspCommonPath = QDir::toNativeSeparators(conf->hspCommonPath());
	update |= m_hspPath       != hspPath;
	update |= m_hspCompPath   != hspCompPath;
	update |= m_hspCommonPath != hspCommonPath;
	m_hspCompPath   = hspPath;
	m_hspPath       = hspCompPath;
	m_hspCommonPath = hspCommonPath;
	if( update ) {
		updateCompilerPath();
	}
}

void CCompiler::hspcmpError(QProcess::ProcessError)
{
}

void CCompiler::hspcmpFinished(int /*exitCode*/, QProcess::ExitStatus exitStatus)
{
	QProcess* hspcmp = qobject_cast<QProcess*>(sender());

	// シグナル発報
	if( QProcess::NormalExit == exitStatus )
	{
		emit compileSuccessful();
	}
	else
	{
		emit compileFailure();
	}

	delete hspcmp;
}

void CCompiler::hspcmpReadOutput()
{
	QProcess* hspcmp = qobject_cast<QProcess*>(sender());
	QString   tmp(hspcmp->readAllStandardOutput());
	emit compileOutput(tmp);
}

// シンボルの取得完了
void CCompiler::listedSymbolsFinished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
{
	QProcess* hspcmp = qobject_cast<QProcess*>(sender());
	QString tmp(hspcmp->readAllStandardOutput());

	m_highlightSymbols.clear();

	for(QStringListIterator ite(tmp.split("\n"));
		ite.hasNext(); )
	{
		QString line = ite.next();
		
		if( !line.contains(",") ) {
			continue;
		}

		// コンパイラのメッセージバッファにHSPで使用されるシンボル名を出力します。
		// 出力は、「シンボル名,sys[|var/func/macro][|1/2]」の形式になります。

		QStringList keywords = line.trimmed().split(QRegExp(",|\\|"));

		for(QMutableStringListIterator ite2(keywords);
			ite2.hasNext(); )
		{
			QString& keyword = ite2.next();
			keyword = keyword.trimmed();
		}

		m_highlightSymbols.push_back(keywords);
	}

//	qDebug() << mSymbols;

	emit updatedSymbols();

	hspcmp->deleteLater();
}

