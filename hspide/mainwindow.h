#include <QMainWindow>
#include <QtGui>
#include <QLocalSocket>
#include <QLocalServer>
#include "workspacemodel.h"
#include "compiler.h"
#include "debugger.h"
#include "outputdock.h"
#include "sysinfodock.h"
#include "varinfodock.h"
#include "projectdock.h"
#include "searchdock.h"
#include "symboldock.h"
#include "messagedock.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class MainWindow : public QMainWindow
{
	Q_OBJECT

	QTabWidget *   tabWidget;
	QProgressBar * taskProgress;

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
	QAction* compileOnlyAct;
	QAction* debugRunAct;
	QAction* noDebugRunAct;
	QAction* debugSuspendAct;
	QAction* debugResumeAct;
	QAction* debugStopAct;
	QAction* settingAct;

	CProjectDock*      projectDock;
	CSymbolDock*       symbolDock;
	COutputDock*       outputDock;
	CSystemInfoDock*   sysInfoDock;
	CVariableInfoDock* varInfoDock;
	CSearchDock*       searchDock;
	CMessageDock*      messageDock;

	QWidget*			m_lastActivatedDocument;

	QMenu*				m_tabListMenu;

	// ソリューション
	CWorkSpaceModel* m_workSpace;

	CCompiler* m_compiler;

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
	void buildStart(const QString & filePath);
	void buildFinished(bool successed);
	void buildOutput(const QString & output);
	void attachedDebugger(CDebugger* debugger);
	void dettachedDebugger();
	void onDocumentChanged();
	void currentTabChanged(int index);
	void onNewFile();
	void onOpenFile();
	void onOpenFile(const QString & filePath);
	void onOpenFile(CWorkSpaceItem * item);
	void onSaveFile();
	void onSaveAsFile();
	void onSaveAllFile();
	void onQuit();
	void onGoToLine();
	void onGoToLine(const QUuid & uuid, int lineNo);
	void onUpdateDebugInfo(const QVector<QPair<QString,QString> > & info);
	void onUpdateVarInfo(const QVector<VARIABLE_INFO_TYPE> & info);
	void onReqVarInfo(const QString& varName, int* info);
	void onBuildProject();
	void onBuildSolution();
	void onDebugRun();
	void onNoDebugRun();
	void onDebugSuspend();
	void onDebugResume();
	void onDebugStop();
	void onOpenSettingDialog();
	void onTabListShow();
	void onTabListClicked();
	void onTabList();
	void onTabClose();
	void clipboardDataChanged();
	void onSymbolDockVisibilityChanged(bool visible);

private:

	void setupDockWindows();
	void setupStatusBar();
	void setupToolBars();
	void setupMenus();
	void setupActions();

	void loadSettings();
	void saveSettings();

	bool isDebugging() const;
	void beginDebugging();
	void endDebugging();

};
