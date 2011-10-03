#include <QMainWindow>
#include <QtGui>
#include "solution.h"
#include "compiler.h"
#include "outputdock.h"
#include "debuggerdock.h"
#include "projectdock.h"
#include "searchdock.h"
#include "symboldock.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	QTabWidget * tabWidget;
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

	// É\ÉäÉÖÅ[ÉVÉáÉì
	CSolution*	mSolution;

	CCompiler*	mCompiler;

public:
	MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);

protected:
	void showEvent(QShowEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

public slots:
	void actionTriggered(QAction *action);
	void buildStart();
	void buildFinished(bool successed);
	void buildOutput(const QString & output);
	void currentTabChanged(int index);
	void onNewFile();
	void onOpenFile(const QString & filePath);
	void onSaveFile();
	void onSaveAsFile();
	void onSaveAllFile();
	void onQuit();
	void onDebugRun();

private:

	void setupDockWindows();
	void setupStatusBar();
	void setupToolBars();
	void setupMenus();
	void setupActions();

//	void newAction();
};
