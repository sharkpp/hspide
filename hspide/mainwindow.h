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

//	typedef enum {
//		ACTION_NEW_DOCUMENT,		// �V�K
//		ACTION_OPEN_DOCUMENT,		// �J��
//		ACTION_SAVE_DOCUMENT,		// �ۑ�
//		ACTION_SAVE_AS_DOCUMENT,	// ���O��t���ĕۑ�
//		ACTION_SAVE_ALL_DOCUMENT,	// �S�ĕۑ�
//		ACTION_QUIT_APPLICATION,	// �I��
//		ACTION_EDIT_UNDO,			// ���ɖ߂�
//		ACTION_EDIT_REDO,			// ��蒼��
//		ACTION_EDIT_CUT,			// �؂���
//		ACTION_EDIT_COPY,			// �R�s�[
//		ACTION_EDIT_PASTE,			// �\��t��
//		ACTION_EDIT_CLEAR,			// �폜
//		ACTION_SELECT_ALL,			// �S�đI��
//		ACTION_FIND_TEXT,			// ����
//		ACTION_FIND_PREV_TEXT,		// �O������
//		ACTION_FIND_NEXT_TEXT,		// ��������
//		ACTION_REPLACE_TEXT,		// �u��
//		ACTION_BUILD_SOLUTION,		// �\�����[�V�������r���h
//		ACTION_BUILD_PROJECT,		// �v���W�F�N�g���r���h
//		ACTION_COMPILE,				// �R���p�C��
//		ACTION_EXECUTE,				// ���s
//	} ACTION_TYPE_ENUM;

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

	// �\�����[�V����
	CSolution*	mSolution;

	CCompiler*	mCompiler;

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

	void loadSettings();
	void saveSettings();

//	void newAction();
};
