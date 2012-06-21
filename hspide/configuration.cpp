#include "configuration.h"
#include <QtGui>

bool operator == (const Configuration::ShortcutInfoType& lhs, const Configuration::ShortcutInfoType& rhs)
{
	return lhs.keys == rhs.keys;
}

bool operator == (const Configuration::ShortcutPresetInfoType& lhs, const Configuration::ShortcutPresetInfoType& rhs)
{
	return lhs.name == rhs.name && lhs.shortcut == rhs.shortcut;
}

Configuration::Configuration()
	: m_hspPath("./")
	, m_hspCommonPath("./common/")
	, m_compilerPath("./")
	, m_editorTabWidth(4)
	, m_editorLineNumberVisibled(true)
	, m_editorLineNumberFontName(QApplication::font().family())
	, m_editorLineNumberFontSize(16)
	, m_editorFontName(QApplication::font().family())
	, m_editorFontSize(16)
{
	for(int i = 0; i < ColorMetricsNum; i++) {
		m_colorMetrics[i].enable = false;
		m_colorMetrics[i].useBoldFont = false;
		m_colorMetrics[i].foregroundColor = Qt::black;
		m_colorMetrics[i].backgroundColor = QColor(255,250,240);
	}

	m_colorMetrics[LabelMetrics].foregroundColor        = Qt::darkYellow;
	m_colorMetrics[CommentMetrics].foregroundColor      = Qt::darkGreen;
	m_colorMetrics[StringMetrics].foregroundColor       = Qt::darkRed;
	m_colorMetrics[FunctionMetrics].foregroundColor     = Qt::blue;
	m_colorMetrics[SubroutineMetrics].foregroundColor   = Qt::blue;
	m_colorMetrics[PreprocessorMetrics].foregroundColor = Qt::blue;
	m_colorMetrics[MacroMetrics].foregroundColor        = QColor(0,128,255);
	m_colorMetrics[TabCharMetrics].foregroundColor      = Qt::lightGray;
	m_colorMetrics[EofMetrics].foregroundColor          = Qt::white;
	m_colorMetrics[EofMetrics].backgroundColor          = Qt::black;

	m_shortcutInfo.resize(2);
	m_shortcutInfo[0].shortcut.resize(ShortcutNum);
	m_shortcutInfo[0].shortcut[ShortcutNew].keys                = QKeySequence::New;
	m_shortcutInfo[0].shortcut[ShortcutOpen].keys               = QKeySequence::Open;
	m_shortcutInfo[0].shortcut[ShortcutSave].keys               = QKeySequence::Save;
	m_shortcutInfo[0].shortcut[ShortcutSaveAs].keys             = QKeySequence::SaveAs;
//	m_shortcutInfo[0].shortcut[ShortcutSaveAll].keys            = 
	m_shortcutInfo[0].shortcut[ShortcutQuit].keys               = QKeySequence::Quit;
	m_shortcutInfo[0].shortcut[ShortcutUndo].keys               = QKeySequence::Undo;
	m_shortcutInfo[0].shortcut[ShortcutRedo].keys               = QKeySequence::Redo;
	m_shortcutInfo[0].shortcut[ShortcutCut].keys                = QKeySequence::Cut;
	m_shortcutInfo[0].shortcut[ShortcutCopy].keys               = QKeySequence::Copy;
	m_shortcutInfo[0].shortcut[ShortcutPaste].keys              = QKeySequence::Paste;
	m_shortcutInfo[0].shortcut[ShortcutClear].keys              = QKeySequence::Delete;
	m_shortcutInfo[0].shortcut[ShortcutSelectAll].keys          = QKeySequence::SelectAll;
	m_shortcutInfo[0].shortcut[ShortcutFind].keys               = QKeySequence::Find;
	m_shortcutInfo[0].shortcut[ShortcutFindNext].keys           = QKeySequence::FindNext;
	m_shortcutInfo[0].shortcut[ShortcutFindPrev].keys           = QKeySequence::FindPrevious;
	m_shortcutInfo[0].shortcut[ShortcutReplace].keys            = QKeySequence::Replace;
	m_shortcutInfo[0].shortcut[ShortcutJump].keys               = QKeySequence(Qt::CTRL + Qt::Key_J);
	m_shortcutInfo[0].shortcut[ShortcutBuildSolution].keys      = QKeySequence(Qt::Key_F7);
	m_shortcutInfo[0].shortcut[ShortcutBuildProject].keys       = QKeySequence(Qt::CTRL + Qt::Key_F7);
//	m_shortcutInfo[0].shortcut[ShortcutCompileOnly].keys        = 
//	m_shortcutInfo[0].shortcut[ShortcutBatchBuild].keys         = 
	m_shortcutInfo[0].shortcut[ShortcutDebugRunSolution].keys   = QKeySequence(Qt::Key_F5);
	m_shortcutInfo[0].shortcut[ShortcutNoDebugRunSolution].keys = QKeySequence(Qt::CTRL + Qt::Key_F5);
	m_shortcutInfo[0].shortcut[ShortcutDebugRunProject].keys    = QKeySequence(Qt::ALT + Qt::Key_F5);
//	m_shortcutInfo[0].shortcut[ShortcutNoDebugRunProject].keys  = 
	m_shortcutInfo[0].shortcut[ShortcutDebugSuspend].keys       = QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_Pause);
	m_shortcutInfo[0].shortcut[ShortcutDebugResume].keys        = QKeySequence(Qt::Key_F5);
	m_shortcutInfo[0].shortcut[ShortcutDebugStop].keys          = QKeySequence(Qt::SHIFT + Qt::Key_F5);
	m_shortcutInfo[0].shortcut[ShortcutConfig].keys             = QKeySequence::Preferences;
	m_shortcutInfo[0].shortcut[ShortcutShowProject].keys        = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_P);
	m_shortcutInfo[0].shortcut[ShortcutShowSymbol].keys         = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Y);
	m_shortcutInfo[0].shortcut[ShortcutShowOutput].keys         = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O);
	m_shortcutInfo[0].shortcut[ShortcutShowSearch].keys         = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
	m_shortcutInfo[0].shortcut[ShortcutShowMessage].keys        = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_M);
	m_shortcutInfo[0].shortcut[ShortcutShowBreakPoint].keys     = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_B);
//	m_shortcutInfo[0].shortcut[ShortcutShowSysInfo].keys        = 
//	m_shortcutInfo[0].shortcut[ShortcutShowVarInfo].keys        = 
	m_shortcutInfo[1].shortcut.resize(ShortcutNum);
	m_shortcutInfo[1] = m_shortcutInfo[0];
	m_shortcutInfo[1].name = tr("HSPED like");
}

Configuration::Configuration(const Configuration& info)
{
	*this = info;
}

Configuration& Configuration::operator = (const Configuration& info)
{
	m_hspPath       = info.m_hspPath;
	m_hspCommonPath = info.m_hspCommonPath;
	m_compilerPath  = info.m_compilerPath;

	m_editorTabWidth = info.m_editorTabWidth;
	m_editorLineNumberVisibled = info.m_editorLineNumberVisibled;

	m_editorLineNumberFontName = info.m_editorLineNumberFontName;
	m_editorLineNumberFontSize = info.m_editorLineNumberFontSize;
	m_editorFontName = info.m_editorFontName;
	m_editorFontSize = info.m_editorFontSize;

	for(int i = 0; i < ColorMetricsNum; i++) {
		m_colorMetrics[i] = info.m_colorMetrics[i];
	}

	m_shortcutInfo  = info.m_shortcutInfo;

	m_tools = info.m_tools;

	emit updateConfiguration(*this);

	return *this;
}

QStringList Configuration::colorMetricsItemNames()
{
	QStringList listItems;
	listItems	<< "text"
				<< "line-number"
				<< "function"
				<< "subroutine"
				<< "preprocessor"
				<< "macro"
				<< "label"
				<< "comment"
				<< "string"
				<< "line-feed-char"
				<< "tab-char"
				<< "eof"
				;
	return listItems;
}

QStringList Configuration::shortcutItemNames()
{
	QStringList listItems;
	listItems	<< "new"
				<< "open"
				<< "save"
				<< "save-as"
				<< "save-all"
				<< "quit"
				<< "undo"
				<< "redo"
				<< "cut"
				<< "copy"
				<< "paste"
				<< "clear"
				<< "select-all"
				<< "find"
				<< "find-next"
				<< "find-prev"
				<< "replace"
				<< "jump"
				<< "build-solution"
				<< "build-project"
				<< "compile"
				<< "batch-build"
				<< "debug-run-solution"
				<< "no-debug-run-solution"
				<< "debug-run-project"
				<< "no-debug-run-project"
				<< "debug-suspend"
				<< "debug-resume"
				<< "debug-stop"
				<< "config"
				<< "show-project"
				<< "show-symbols"
				<< "show-output"
				<< "show-search"
				<< "show-messages"
				<< "show-breakpoints"
				<< "show-system-variables"
				<< "show-variables"
				;
	return listItems;
}

bool Configuration::load(const QSettings& settings)
{
	QVariant tmp;

	// コンパイラパスを取得
	setCompilerPath(settings.value("path/compiler", QDir::currentPath()).toString());
	// HSPディレクトリの取得
	setHspPath(settings.value("path/hsp", QDir::currentPath()).toString());
	// Commonディレクトリの取得
	setHspCommonPath(settings.value("path/hsp-common", hspPath() + "/common/").toString());

	// エディタ関連の設定を取得
	setEditorTabWidth(settings.value("editor/tab-width", editorTabWidth()).toInt());
	setEditorLineNumberVisible(settings.value("editor/line-number-visible", editorLineNumberVisibled()).toBool());
	setEditorLineNumberFontName(settings.value("editor/line-number-font-name", editorLineNumberFontName()).toString());
	setEditorLineNumberFontSize(settings.value("editor/line-number-font-size", editorLineNumberFontSize()).toInt());
	setEditorFontName(settings.value("editor/font-name", editorFontName()).toString());
	setEditorFontSize(settings.value("editor/font-size", editorFontSize()).toInt());

	// 色設定
	QStringList listItems = colorMetricsItemNames();
	for(int i = 0; i < listItems.size(); i++)
	{
		Configuration::ColorMetricsInfoType metrics = colorMetrics(Configuration::ColorMetricsEnum(i));
		metrics.enable          = settings.value(       QString("editor-color/%1-enable"          ).arg(listItems.at(i)), metrics.enable).toBool();
		metrics.useBoldFont     = settings.value(       QString("editor-color/%1-use-bold-font"   ).arg(listItems.at(i)), metrics.useBoldFont).toBool();
		metrics.backgroundColor = QColor(settings.value(QString("editor-color/%1-foreground-color").arg(listItems.at(i)), metrics.backgroundColor.name()).toString());
		metrics.foregroundColor = QColor(settings.value(QString("editor-color/%1-background-color").arg(listItems.at(i)), metrics.foregroundColor.name()).toString());
		setColorMetrics(Configuration::ColorMetricsEnum(i), metrics);
	}

	// キー割り当て
	QVector<ShortcutPresetInfoType> oldShortcutInfo = m_shortcutInfo;
	QStringList listShortcutItems = shortcutItemNames();
	for(int i = 0, presetNum = 0; i < 1 + presetNum; i++)
	{
		QString presetTitle;
		if( 0 == i )
		{
			presetNum = settings.value("key-assign/preset-num", 0).toInt();
			m_shortcutInfo.resize(1 + presetNum);
		}
		else
		{
			QString key = QString("key-assign/%1/preset-name").arg(i - 1);
			presetTitle = settings.value(key, tr("untitled")).toString();
		}

		m_shortcutInfo[i].name = presetTitle;
		m_shortcutInfo[i].shortcut.resize(ShortcutNum);

		for(int j = 0; j < listShortcutItems.size(); j++)
		{
			QString key = 0 == i
							? QString("key-assign/%1")
								.arg(listShortcutItems.at(j))
							: QString("key-assign/%1/%2")
								.arg(i - 1)
								.arg(listShortcutItems.at(j));
			QString value = i < oldShortcutInfo.size()
								? oldShortcutInfo[i].shortcut[j].keys.toString()
								: "";
			ShortcutInfoType info;
			info.keys = QKeySequence(settings.value(key, value).toString());
			setShortcut(i - 1, Configuration::ShortcutEnum(j), info);
		}
	}

	emit updateConfiguration(*this);

	return true;
}

bool Configuration::save(QSettings& settings) const
{
	QString tmp;

	// コンパイラパスを保存
	settings.setValue("path/compiler", compilerPath());
	// HSPディレクトリの保存
	settings.setValue("path/hsp", hspPath());
	// Commonディレクトリの保存
	settings.setValue("path/hsp-common", hspCommonPath());

	// エディタ関連の設定を保存
	settings.setValue("editor/tab-width", editorTabWidth());
	settings.setValue("editor/line-number-visible", editorLineNumberVisibled());
	settings.setValue("editor/line-number-font-name", editorLineNumberFontName());
	settings.setValue("editor/line-number-font-size", editorLineNumberFontSize());
	settings.setValue("editor/font-name", editorFontName());
	settings.setValue("editor/font-size", editorFontSize());

	// 色設定
	QStringList listItems = colorMetricsItemNames();
	for(int i = 0; i < listItems.size(); i++)
	{
		const Configuration::ColorMetricsInfoType& metrics = colorMetrics(Configuration::ColorMetricsEnum(i));
		settings.setValue(QString("editor-color/%1-enable"          ).arg(listItems.at(i)), metrics.enable);
		settings.setValue(QString("editor-color/%1-use-bold-font"   ).arg(listItems.at(i)), metrics.useBoldFont);
		settings.setValue(QString("editor-color/%1-foreground-color").arg(listItems.at(i)), metrics.backgroundColor);
		settings.setValue(QString("editor-color/%1-background-color").arg(listItems.at(i)), metrics.foregroundColor);
	}

	// キー割り当て
	QStringList listShortcutItems = shortcutItemNames();
	for(int i = 0, presetNum = m_shortcutInfo.size() - 1;
		i < 1 + presetNum; i++)
	{
		QString presetTitle;
		if( 0 == i )
		{
			settings.setValue("key-assign/preset-num", presetNum);
		}
		else
		{
			QString key = QString("key-assign/%1/preset-name").arg(i - 1);
			settings.setValue(key, m_shortcutInfo[i+1].name);
		}

		for(int j = 0; j < listShortcutItems.size(); j++)
		{
			const Configuration::ShortcutInfoType& info = m_shortcutInfo[i].shortcut[j];
			QString key = 0 == i
							? QString("key-assign/%1")
								.arg(listShortcutItems.at(j))
							: QString("key-assign/%1/%2")
								.arg(i - 1)
								.arg(listShortcutItems.at(j));
			settings.setValue(key, info.keys.toString());
		}
	}

	return true;
}

int Configuration::currentShortcutPreset() const
{
	for(int i = 1, presetNum = m_shortcutInfo.size() - 1;
		i < presetNum; i++)
	{
		if( m_shortcutInfo[0] == m_shortcutInfo[i] ) 
		{
			return i - 1;
		}
	}
	return -1;
}
