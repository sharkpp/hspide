
#include <QMainWindow>
#include <QtGui>
#include "solution.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	QTabWidget * tabWidget;
	QProgressBar * taskProgress;

	QAction *newDocumentAct;
	QAction *openDocumentAct;
	QAction *saveDocumentAct;
	QAction *saveAsDocumentAct;
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

	QSharedPointer<CSolution>	mSolution;

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
	void onOpenFile(const QString & filePath);
	void onSaveFile();
	void onSaveAsFile();
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
