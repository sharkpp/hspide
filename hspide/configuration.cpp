#include "configuration.h"
#include <QtGui>

bool operator == (const Configuration::KeyAssignInfoType& lhs, const Configuration::KeyAssignInfoType& rhs)
{
	return lhs.keys == rhs.keys;
}

bool operator == (const Configuration::KeyAssignPresetInfoType& lhs, const Configuration::KeyAssignPresetInfoType& rhs)
{
	return lhs.name == rhs.name && lhs.keyAssign == rhs.keyAssign;
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

	m_keyAssignInfo.resize(2);
	m_keyAssignInfo[0].keyAssign.resize(ShortcutNum);
	m_keyAssignInfo[0].keyAssign[ShortcutNew].keys                = QKeySequence::New;
	m_keyAssignInfo[0].keyAssign[ShortcutOpen].keys               = QKeySequence::Open;
	m_keyAssignInfo[0].keyAssign[ShortcutSave].keys               = QKeySequence::Save;
	m_keyAssignInfo[0].keyAssign[ShortcutSaveAs].keys             = QKeySequence::SaveAs;
//	m_keyAssignInfo[0].keyAssign[ShortcutSaveAll].keys            = 
	m_keyAssignInfo[0].keyAssign[ShortcutQuit].keys               = QKeySequence::Quit;
	m_keyAssignInfo[0].keyAssign[ShortcutUndo].keys               = QKeySequence::Undo;
	m_keyAssignInfo[0].keyAssign[ShortcutRedo].keys               = QKeySequence::Redo;
	m_keyAssignInfo[0].keyAssign[ShortcutCut].keys                = QKeySequence::Cut;
	m_keyAssignInfo[0].keyAssign[ShortcutCopy].keys               = QKeySequence::Copy;
	m_keyAssignInfo[0].keyAssign[ShortcutPaste].keys              = QKeySequence::Paste;
	m_keyAssignInfo[0].keyAssign[ShortcutClear].keys              = QKeySequence::Delete;
	m_keyAssignInfo[0].keyAssign[ShortcutSelectAll].keys          = QKeySequence::SelectAll;
	m_keyAssignInfo[0].keyAssign[ShortcutFind].keys               = QKeySequence::Find;
	m_keyAssignInfo[0].keyAssign[ShortcutFindNext].keys           = QKeySequence::FindNext;
	m_keyAssignInfo[0].keyAssign[ShortcutFindPrev].keys           = QKeySequence::FindPrevious;
	m_keyAssignInfo[0].keyAssign[ShortcutReplace].keys            = QKeySequence::Replace;
	m_keyAssignInfo[0].keyAssign[ShortcutJump].keys               = QKeySequence(Qt::CTRL + Qt::Key_J);
	m_keyAssignInfo[0].keyAssign[ShortcutBuildSolution].keys      = QKeySequence(Qt::Key_F7);
	m_keyAssignInfo[0].keyAssign[ShortcutBuildProject].keys       = QKeySequence(Qt::CTRL + Qt::Key_F7);
//	m_keyAssignInfo[0].keyAssign[ShortcutCompileOnly].keys        = 
//	m_keyAssignInfo[0].keyAssign[ShortcutBatchBuild].keys         = 
	m_keyAssignInfo[0].keyAssign[ShortcutDebugRunSolution].keys   = QKeySequence(Qt::Key_F5);
	m_keyAssignInfo[0].keyAssign[ShortcutNoDebugRunSolution].keys = QKeySequence(Qt::CTRL + Qt::Key_F5);
	m_keyAssignInfo[0].keyAssign[ShortcutDebugRunProject].keys    = QKeySequence(Qt::ALT + Qt::Key_F5);
//	m_keyAssignInfo[0].keyAssign[ShortcutNoDebugRunProject].keys  = 
	m_keyAssignInfo[0].keyAssign[ShortcutDebugSuspend].keys       = QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_Pause);
	m_keyAssignInfo[0].keyAssign[ShortcutDebugResume].keys        = QKeySequence(Qt::Key_F5);
	m_keyAssignInfo[0].keyAssign[ShortcutDebugStop].keys          = QKeySequence(Qt::SHIFT + Qt::Key_F5);
	m_keyAssignInfo[0].keyAssign[ShortcutConfig].keys             = QKeySequence::Preferences;
	m_keyAssignInfo[0].keyAssign[ShortcutShowProject].keys        = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_P);
	m_keyAssignInfo[0].keyAssign[ShortcutShowSymbol].keys         = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Y);
	m_keyAssignInfo[0].keyAssign[ShortcutShowOutput].keys         = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O);
	m_keyAssignInfo[0].keyAssign[ShortcutShowSearch].keys         = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
	m_keyAssignInfo[0].keyAssign[ShortcutShowMessage].keys        = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_M);
	m_keyAssignInfo[0].keyAssign[ShortcutShowBreakPoint].keys     = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_B);
//	m_keyAssignInfo[0].keyAssign[ShortcutShowSysInfo].keys        = 
//	m_keyAssignInfo[0].keyAssign[ShortcutShowVarInfo].keys        = 
	m_keyAssignInfo[1].keyAssign.resize(ShortcutNum);
	m_keyAssignInfo[1] = m_keyAssignInfo[0];
	m_keyAssignInfo[1].name = tr("hsed3 like");
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

	m_keyAssignInfo  = info.m_keyAssignInfo;

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

QStringList Configuration::keyAssignItemNames()
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
	QVector<KeyAssignPresetInfoType> oldKeyAssignInfo = m_keyAssignInfo;
	QStringList listKeyAssignItems = keyAssignItemNames();
	for(int i = 0, presetNum = 0; i < 1 + presetNum; i++)
	{
		QString presetTitle;
		if( 0 == i )
		{
			presetNum = settings.value("key-assign/preset-num", 0).toInt();
			m_keyAssignInfo.resize(1 + presetNum);
		}
		else
		{
			QString key = QString("key-assign/%1/preset-name").arg(i - 1);
			presetTitle = settings.value(key, tr("untitled")).toString();
		}

		m_keyAssignInfo[i].name = presetTitle;
		m_keyAssignInfo[i].keyAssign.resize(ShortcutNum);

		for(int j = 0; j < listKeyAssignItems.size(); j++)
		{
			QString key = 0 == i
							? QString("key-assign/%1")
								.arg(listKeyAssignItems.at(j))
							: QString("key-assign/%1/%2")
								.arg(i - 1)
								.arg(listKeyAssignItems.at(j));
			QString value = i < oldKeyAssignInfo.size()
								? oldKeyAssignInfo[i].keyAssign[j].keys.toString()
								: "";
			KeyAssignInfoType info;
			info.keys = QKeySequence(settings.value(key, value).toString());
			setKeyAssign(i - 1, Configuration::ShortcutEnum(j), info);
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
	QStringList listKeyAssignItems = keyAssignItemNames();
	for(int i = 0, presetNum = m_keyAssignInfo.size() - 1;
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
			settings.setValue(key, m_keyAssignInfo[i].name);
		}

		for(int j = 0; j < listKeyAssignItems.size(); j++)
		{
			const Configuration::KeyAssignInfoType& info = m_keyAssignInfo[i].keyAssign[j];
			QString key = 0 == i
							? QString("key-assign/%1")
								.arg(listKeyAssignItems.at(j))
							: QString("key-assign/%1/%2")
								.arg(i - 1)
								.arg(listKeyAssignItems.at(j));
			settings.setValue(key, info.keys.toString());
		}
	}

	return true;
}

int Configuration::currentKeyAssignPreset() const
{
	for(int i = 1, presetNum = m_keyAssignInfo.size() - 1;
		i < presetNum; i++)
	{
		if( m_keyAssignInfo[0] == m_keyAssignInfo[i] ) 
		{
			return i - 1;
		}
	}
	return -1;
}

void Configuration::appendKeyAssignPreset(const QString& name, const QVector<KeyAssignInfoType>& info)
{
	KeyAssignPresetInfoType newInfo = {
			name, info
		};
	m_keyAssignInfo.append(newInfo);
}

void Configuration::removeKeyAssignPreset(int index)
{
	if( 0 <= index &&
		index < m_keyAssignInfo.size() )
	{
		m_keyAssignInfo.remove(index);
	}
}
