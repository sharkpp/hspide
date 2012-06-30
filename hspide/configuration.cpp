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

bool operator == (const Configuration::ToolbarPresetInfoType& lhs, const Configuration::ToolbarPresetInfoType& rhs)
{
	return lhs.toolbar == rhs.toolbar;
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
	m_keyAssignInfo[0].keyAssign.resize(ActionNum);
	m_keyAssignInfo[0].keyAssign[ActionNew].keys                = QKeySequence::New;
	m_keyAssignInfo[0].keyAssign[ActionOpen].keys               = QKeySequence::Open;
	m_keyAssignInfo[0].keyAssign[ActionSave].keys               = QKeySequence::Save;
	m_keyAssignInfo[0].keyAssign[ActionSaveAs].keys             = QKeySequence::SaveAs;
//	m_keyAssignInfo[0].keyAssign[ActionSaveAll].keys            = 
	m_keyAssignInfo[0].keyAssign[ActionQuit].keys               = QKeySequence::Quit;
	m_keyAssignInfo[0].keyAssign[ActionUndo].keys               = QKeySequence::Undo;
	m_keyAssignInfo[0].keyAssign[ActionRedo].keys               = QKeySequence::Redo;
	m_keyAssignInfo[0].keyAssign[ActionCut].keys                = QKeySequence::Cut;
	m_keyAssignInfo[0].keyAssign[ActionCopy].keys               = QKeySequence::Copy;
	m_keyAssignInfo[0].keyAssign[ActionPaste].keys              = QKeySequence::Paste;
	m_keyAssignInfo[0].keyAssign[ActionClear].keys              = QKeySequence::Delete;
	m_keyAssignInfo[0].keyAssign[ActionSelectAll].keys          = QKeySequence::SelectAll;
	m_keyAssignInfo[0].keyAssign[ActionFind].keys               = QKeySequence::Find;
	m_keyAssignInfo[0].keyAssign[ActionFindNext].keys           = QKeySequence::FindNext;
	m_keyAssignInfo[0].keyAssign[ActionFindPrev].keys           = QKeySequence::FindPrevious;
	m_keyAssignInfo[0].keyAssign[ActionReplace].keys            = QKeySequence::Replace;
	m_keyAssignInfo[0].keyAssign[ActionJump].keys               = QKeySequence(Qt::CTRL + Qt::Key_J);
	m_keyAssignInfo[0].keyAssign[ActionBuildSolution].keys      = QKeySequence(Qt::Key_F7);
	m_keyAssignInfo[0].keyAssign[ActionBuildProject].keys       = QKeySequence(Qt::CTRL + Qt::Key_F7);
//	m_keyAssignInfo[0].keyAssign[ActionCompileOnly].keys        = 
//	m_keyAssignInfo[0].keyAssign[ActionBatchBuild].keys         = 
	m_keyAssignInfo[0].keyAssign[ActionDebugRunSolution].keys   = QKeySequence(Qt::Key_F5);
	m_keyAssignInfo[0].keyAssign[ActionNoDebugRunSolution].keys = QKeySequence(Qt::CTRL + Qt::Key_F5);
	m_keyAssignInfo[0].keyAssign[ActionDebugRunProject].keys    = QKeySequence(Qt::ALT + Qt::Key_F5);
//	m_keyAssignInfo[0].keyAssign[ActionNoDebugRunProject].keys  = 
	m_keyAssignInfo[0].keyAssign[ActionDebugSuspend].keys       = QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_Pause);
	m_keyAssignInfo[0].keyAssign[ActionDebugResume].keys        = QKeySequence(Qt::Key_F5);
	m_keyAssignInfo[0].keyAssign[ActionDebugStop].keys          = QKeySequence(Qt::SHIFT + Qt::Key_F5);
	m_keyAssignInfo[0].keyAssign[ActionConfig].keys             = QKeySequence::Preferences;
	m_keyAssignInfo[0].keyAssign[ActionShowProject].keys        = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_P);
	m_keyAssignInfo[0].keyAssign[ActionShowSymbol].keys         = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Y);
	m_keyAssignInfo[0].keyAssign[ActionShowOutput].keys         = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O);
	m_keyAssignInfo[0].keyAssign[ActionShowSearch].keys         = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
	m_keyAssignInfo[0].keyAssign[ActionShowMessage].keys        = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_M);
	m_keyAssignInfo[0].keyAssign[ActionShowBreakPoint].keys     = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_B);
//	m_keyAssignInfo[0].keyAssign[ActionShowSysInfo].keys        = 
//	m_keyAssignInfo[0].keyAssign[ActionShowVarInfo].keys        = 
	m_keyAssignInfo[1].keyAssign.resize(ActionNum);
	m_keyAssignInfo[1] = m_keyAssignInfo[0];
	m_keyAssignInfo[1].name = tr("hsed3 like");

	m_toolbarInfo.resize(2);
	m_toolbarInfo[0].toolbar.push_back(ActionOpen);
	m_toolbarInfo[0].toolbar.push_back(ActionSave);
	m_toolbarInfo[0].toolbar.push_back(ActionSaveAll);
	m_toolbarInfo[0].toolbar.push_back(ActionSeparator);
	m_toolbarInfo[0].toolbar.push_back(ActionCut);
	m_toolbarInfo[0].toolbar.push_back(ActionCopy);
	m_toolbarInfo[0].toolbar.push_back(ActionPaste);
	m_toolbarInfo[0].toolbar.push_back(ActionSeparator);
	m_toolbarInfo[0].toolbar.push_back(ActionUndo);
	m_toolbarInfo[0].toolbar.push_back(ActionRedo);
	m_toolbarInfo[0].toolbar.push_back(ActionSeparator);
	m_toolbarInfo[0].toolbar.push_back(ActionFind);
	m_toolbarInfo[0].toolbar.push_back(ActionFindNext);
	m_toolbarInfo[0].toolbar.push_back(ActionFindPrev);
	m_toolbarInfo[0].toolbar.push_back(ActionReplace);
	m_toolbarInfo[0].toolbar.push_back(ActionSeparator);
	m_toolbarInfo[0].toolbar.push_back(ActionDebugRunSolution);
	m_toolbarInfo[0].toolbar.push_back(ActionDebugSuspend);
	m_toolbarInfo[0].toolbar.push_back(ActionDebugResume);
	m_toolbarInfo[0].toolbar.push_back(ActionDebugStop);
	m_toolbarInfo[1] = m_toolbarInfo[0];
	m_toolbarInfo[1].name = tr("hsed3 like");
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

	m_toolbarInfo = info.m_toolbarInfo;

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

QStringList Configuration::actionItemNames()
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
				<< "about"
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
	QStringList listKeyAssignItems = actionItemNames();
	for(int i = 0, presetNum = 0; i < 1 + presetNum; i++)
	{
		QString keyBase = 0 == i
						? "key-assign"
						: QString("key-assign/%1")
							.arg(i - 1);
		QString presetTitle;
		if( 0 == i )
		{
			QVariant value = settings.value("key-assign/preset-num");
			if( !value.isValid() ) {
				break; // 設定が存在しない
			}
			presetNum = value.toInt();
			m_keyAssignInfo.resize(1 + presetNum);
		}
		else
		{
			QString key = QString("key-assign/%1/preset-name").arg(i - 1);
			presetTitle = settings.value(key, tr("untitled")).toString();
		}

		m_keyAssignInfo[i].name = presetTitle;
		m_keyAssignInfo[i].keyAssign.resize(ActionNum);

		for(int j = 0; j < listKeyAssignItems.size(); j++)
		{
			QString key = QString("%1/%2")
								.arg(keyBase)
								.arg(listKeyAssignItems.at(j));
			QString defaultValue = i < oldKeyAssignInfo.size()
										? oldKeyAssignInfo[i].keyAssign[j].keys.toString()
										: "";
			KeyAssignInfoType info;
			info.keys = QKeySequence(settings.value(key, defaultValue).toString());
			setKeyAssign(i - 1, Configuration::ActionEnum(j), info);
		}
	}

	// ツールバー
	QVector<ToolbarPresetInfoType> oldToolbarInfo = m_toolbarInfo;
	listKeyAssignItems.push_back("----"); // セパレータ
	for(int i = 0, presetNum = 0; i < 1 + presetNum; i++)
	{
		QString keyBase = 0 == i
						? "toolbar"
						: QString("toolbar/%1")
							.arg(i - 1);
		QString presetTitle;
		int buttonNum = 0;
		if( 0 == i )
		{
			QVariant value = settings.value("toolbar/preset-num");
			if( !value.isValid() ) {
				break; // 設定が存在しない
			}
			presetNum = value.toInt();
			m_toolbarInfo.resize(1 + presetNum);
		}
		else
		{
			QString key = QString("toolbar/%1/preset-name").arg(i - 1);
			presetTitle = settings.value(key, tr("untitled")).toString();
		}

		buttonNum = settings.value(QString("%1/button-num").arg(keyBase), 0).toInt();

		m_toolbarInfo[i].name = presetTitle;
		m_toolbarInfo[i].toolbar.clear();

		for(int j = 0; j < buttonNum; j++)
		{
			QString key = QString("%1/button-%2")
								.arg(keyBase)
								.arg(j);
			QString defaultValue = i < oldToolbarInfo.size() && j < oldToolbarInfo[i].toolbar.size()
										? listKeyAssignItems[oldToolbarInfo[i].toolbar[j]]
										: "";
			QString value = settings.value(key, defaultValue).toString();
			int k = listKeyAssignItems.indexOf(value);
			if( 0 <= k && k <= Configuration::ActionNum ) {
				m_toolbarInfo[i].toolbar.push_back(Configuration::ActionEnum(k));
			}
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
	QStringList listKeyAssignItems = actionItemNames();
	for(int i = 0, presetNum = m_keyAssignInfo.size() - 1;
		i < 1 + presetNum; i++)
	{
		QString keyBase = 0 == i
						? "key-assign"
						: QString("key-assign/%1")
							.arg(i - 1);
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
			QString key = QString("%1/%2")
								.arg(keyBase)
								.arg(listKeyAssignItems.at(j));
			settings.setValue(key, info.keys.toString());
		}
	}

	// ツールバー
	listKeyAssignItems.push_back("----"); // セパレータ
	for(int i = 0, presetNum = m_toolbarInfo.size() - 1;
		i < 1 + presetNum; i++)
	{
		QString keyBase = 0 == i
						? "toolbar"
						: QString("toolbar/%1")
							.arg(i - 1);
		QString presetTitle;
		if( 0 == i )
		{
			settings.setValue("toolbar/preset-num", presetNum);
		}
		else
		{
			settings.setValue(QString("toolbar/%1/preset-name").arg(i - 1),
			                  m_toolbarInfo[i].name);
		}

		settings.setValue(QString("%1/button-num").arg(keyBase),
		                  m_toolbarInfo[i].toolbar.size());

		for(int j = 0; j < m_toolbarInfo[i].toolbar.size(); j++)
		{
			ActionEnum action = m_toolbarInfo[i].toolbar[j];
			QString key = QString("%1/button-%2")
								.arg(keyBase)
								.arg(j);
			settings.setValue(key, listKeyAssignItems[action]);
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
	if( 0 < index &&
		index < m_keyAssignInfo.size() )
	{
		m_keyAssignInfo.remove(index);
	}
}

int Configuration::currentToolbarPreset() const
{
	for(int i = 1, presetNum = m_toolbarInfo.size() - 1;
		i < presetNum; i++)
	{
		if( m_toolbarInfo[0] == m_toolbarInfo[i] ) 
		{
			return i - 1;
		}
	}
	return -1;
}

void Configuration::appendToolbarPreset(const QString& name, const QVector<ActionEnum>& info)
{
	ToolbarPresetInfoType newInfo = {
			name, info
		};
	m_toolbarInfo.append(newInfo);
}

void Configuration::removeToolbarPreset(int index)
{
	if( 0 < index &&
		index < m_toolbarInfo.size() )
	{
		m_toolbarInfo.remove(index);
	}
}
