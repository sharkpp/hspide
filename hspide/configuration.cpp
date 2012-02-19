#include "configuration.h"
#include <QtGui>

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

	m_shortcutInfo[ShortcutNew].keys             = QKeySequence::New;
	m_shortcutInfo[ShortcutOpen].keys            = QKeySequence::Open;
	m_shortcutInfo[ShortcutSave].keys            = QKeySequence::Save;
	m_shortcutInfo[ShortcutSaveAs].keys          = QKeySequence::SaveAs;
//	m_shortcutInfo[ShortcutSaveAll].keys         = ;
	m_shortcutInfo[ShortcutQuit].keys            = QKeySequence::Quit;
	m_shortcutInfo[ShortcutUndo].keys            = QKeySequence::Undo;
	m_shortcutInfo[ShortcutRedo].keys            = QKeySequence::Redo;
	m_shortcutInfo[ShortcutCut].keys             = QKeySequence::Cut;
	m_shortcutInfo[ShortcutCopy].keys            = QKeySequence::Copy;
	m_shortcutInfo[ShortcutPaste].keys           = QKeySequence::Paste;
	m_shortcutInfo[ShortcutClear].keys           = QKeySequence::Delete;
	m_shortcutInfo[ShortcutSelectAll].keys       = QKeySequence::SelectAll;
	m_shortcutInfo[ShortcutFind].keys            = QKeySequence::Find;
	m_shortcutInfo[ShortcutFindNext].keys        = QKeySequence::FindNext;
	m_shortcutInfo[ShortcutFindPrev].keys        = QKeySequence::FindPrevious;
	m_shortcutInfo[ShortcutReplace].keys         = QKeySequence::Replace;
	m_shortcutInfo[ShortcutJump].keys            = QKeySequence(Qt::CTRL + Qt::Key_J);
	m_shortcutInfo[ShortcutDebugRun].keys        = QKeySequence(Qt::Key_F5);
//	m_shortcutInfo[ShortcutConfig].keys          = ;
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

	for(int i = 0; i < ShortcutNum; i++) {
		m_shortcutInfo[i] = info.m_shortcutInfo[i];
	}

	m_tools = info.m_tools;

	emit updateConfiguration(*this);

	return *this;
}

