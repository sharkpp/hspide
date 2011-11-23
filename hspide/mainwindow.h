#include <QMainWindow>
#include <QtGui>
#include "workspacemodel.h"
#include "compiler.h"
#include "outputdock.h"
#include "debuggerdock.h"
#include "projectdock.h"
#include "searchdock.h"
#include "symboldock.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class MainWindow : public QMainWindow
{
	Q_OBJECT

	QTabWidget *   tabWidget;
	QProgressBar * taskProgress;

	QAction *newDocumentAct;
	QAction *openDocumentAct;
	QAction *saveDocumentAct;
	QAction *saveAsDocumentAct;
	QAction *saveAllDocumentAct;
	QAction *quitApplicationAct;
	QAction *editUndoAct;
	QAction *editRedoAct;
	QAction *editCutAct;
	QAction *editCopyAct;
	QAction *editPasteAct;
	QAction *editClearAct;
	QAction *selectAllAct;
	QAction *findTextAct;
	QAction *findPrevTextAct;
	QAction *findNextTextAct;
	QAction *replaceTextAct;
	QAction *buildSolutionAct;
	QAction *buildProjectAct;
	QAction *compileOnlyAct;
	QAction *debugRunAct;
	QAction *noDebugRunAct;

	CProjectDock *  projectDock;
	CSymbolDock *   symbolDock;
	COutputDock *   outputDock;
	CDebuggerDock * debuggerDock;
	CSearchDock *   searchDock;

	QWidget* m_lastActivatedDocument;

	// ƒ\ƒŠƒ…[ƒVƒ‡ƒ“
	CWorkSpaceModel* m_workSpace;

	CCompiler* m_compiler;

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
	void onBuildProject();
	void onBuildSolution();
	void onDebugRun();
	void onTabList();
	void onTabClose();

private:

	void setupDockWindows();
	void setupStatusBar();
	void setupToolBars();
	void setupMenus();
	void setupActions();

	void loadSettings();
	void saveSettings();
};
