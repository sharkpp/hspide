
#include <QMainWindow>

class MainWindow : public QMainWindow
{
	Q_OBJECT

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

public:
	MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);

protected:
	void showEvent(QShowEvent *event);

public slots:
	void actionTriggered(QAction *action);

private:

	void setupDockWindows();
	void setupStatusBar();
	void setupToolBars();
	void setupMenus();
	void setupActions();

//	void newAction();
};
