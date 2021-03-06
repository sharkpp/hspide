#include <QMainWindow>
#include <QtGui>
#include <QLocalSocket>
#include <QLocalServer>
#include "workspacemodel.h"
#include "compilerset.h"
#include "compiler.h"
#include "debugger.h"
#include "outputdock.h"
#include "sysinfodock.h"
#include "varinfodock.h"
#include "workspacedock.h"
#include "searchdock.h"
#include "symboldock.h"
#include "messagedock.h"
#include "breakpointdock.h"
#include "global.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class MainWindow : public QMainWindow
{
	Q_OBJECT

	QTabWidget*   tabWidget;
	QProgressBar* taskProgress;

	QAction* newDocumentAct;
	QAction* openDocumentAct;
	QAction* saveDocumentAct;
	QAction* saveAsDocumentAct;
	QAction* saveAllDocumentAct;
	QAction* quitApplicationAct;
	QAction* editUndoAct;
	QAction* editRedoAct;
	QAction* editCutAct;
	QAction* editCopyAct;
	QAction* editPasteAct;
	QAction* editClearAct;
	QAction* selectAllAct;
	QAction* findTextAct;
	QAction* findPrevTextAct;
	QAction* findNextTextAct;
	QAction* replaceTextAct;
	QAction* gotoLineAct;
	QAction* buildSolutionAct;
	QAction* buildProjectAct;
	QAction* batchBuildAct;
	QAction* compileOnlyAct;
	QAction* debugRunSolutionAct;
	QAction* noDebugRunSolutionAct;
	QAction* debugRunProjectAct;
	QAction* noDebugRunProjectAct;
	QAction* debugSuspendAct;
	QAction* debugResumeAct;
	QAction* debugStopAct;
	QAction* debugStepInAct;
	QAction* debugStepOverAct;
	QAction* debugStepOutAct;
	QAction* breakpointSetAct;
	QAction* buildTargetAct;
	QAction* settingAct;
	QAction* aboutAct;
	QAction* showWorkSpaceDockAct;
	QAction* showSymbolDockAct;
	QAction* showOutputDockAct;
	QAction* showSearchDockAct;
	QAction* showMessageDockAct;
	QAction* showBreakPointDockAct;
	QAction* showSysInfoDockAct;
	QAction* showVarInfoDockAct;

	CWorkSpaceDock*    workSpaceDock;
	CSymbolDock*       symbolDock;
	COutputDock*       outputDock;
	CSystemInfoDock*   sysInfoDock;
	CVariableInfoDock* varInfoDock;
	CSearchDock*       searchDock;
	CMessageDock*      messageDock;
	CBreakPointDock*   breakPointDock;

	QToolBar*	m_generalToolbar;

	QWidget*	m_lastActivatedDocument;

	QMenu*		m_tabListMenu;

	QComboBox*	m_buildConf;

	// ソリューション
	CWorkSpaceModel* m_workSpace;

	CCompilerSet* m_compilers;
	CSymbolsList m_symbols;

	QSet<CDebugger*> m_debuggers;

	QByteArray m_stateDefault;
	QByteArray m_stateDebugging;

public:
	MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	virtual ~MainWindow();

protected:
	void showEvent(QShowEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	void closeEvent(QCloseEvent *event);

public slots:

	void actionTriggered(QAction *action);
	void updateConfiguration(const Configuration* conf);
	void buildStarted();
	void buildStarted(int buildOrder,const QUuid& uuid);
	void buildSuccessful(int buildOrder);
	void buildFailure(int buildOrder);
	void buildOutput(int buildOrder, const QString& output);
	void onUpdatedSymbols();
	void attachedDebugger(CDebugger* debugger);
	void dettachedDebugger();
	void onPutDebugLog(const QString& text);
	void onDocumentChanged();
	void onUpdateBreakpoint();
	void currentTabChanged(int index);
	void onAboutApp();
	void onNewFile();
	void onOpenFile();
	void onOpenFile(const QString& filePath);
	void onOpenFile(CWorkSpaceItem* item);
	void onSaveFile();
	void onSaveAsFile();
	void onSaveAllFile();
	void onQuit();
	void onGoToLine();
	void onGoToLine(const QUuid& uuid, int lineNo);
	void onStopAtBreakPoint(const QUuid& uuid, int lineNo);
	void onStopCode(const QUuid& uuid, int lineNo);
	void onUpdateDebugInfo(const QVector<QPair<QString,QString> >& info);
	void onUpdateVarInfo(const QVector<VARIABLE_INFO_TYPE>& info);
	void onReqVarInfo(const QString& varName, int* info);
	void onBuildProject();
	void onBuildSolution();
	void onBatchBuild();
	void onDebugRunSolution();
	void onNoDebugRunSolution();
	void onDebugRunProject();
	void onNoDebugRunProject();
	void onDebugSuspend();
	void onDebugResume();
	void onDebugStop();
	void onDebugStepIn();
	void onDebugStepOver();
	void onDebugStepOut();
	void onBreakpointSet();
	void onOpenSettingDialog();
	void onTabListShow();
	void onTabListClicked();
	void onTabList();
	void onTabClose();
	void clipboardDataChanged();
	void onDocumentChanged(bool);
	void onSymbolDockVisibilityChanged(bool visible);
	void onShowDock();
	void onBuildTargetChanged(int index);
	void onFileChanged(const QUuid& uuid);
	void onUpdateSolution(CWorkSpaceItem* item);

private:

	void setupDockWindows();
	void setupStatusBar();
	void setupToolBars();
	void setupMenus();
	void setupActions();

	void loadSettings();
	void saveSettings();

	void updateTextCursorLine(const QUuid& uuid = QUuid(), int lineNo = -1);

	bool isDebugging() const;
	void beginDebugging();
	void endDebugging();

	void closeTab(CWorkSpaceItem* targetItem = NULL);

	bool closeSolution();

	CWorkSpaceItem* currentItem();
};
