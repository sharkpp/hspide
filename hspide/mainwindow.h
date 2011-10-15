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
//		ACTION_NEW_DOCUMENT,		// 新規
//		ACTION_OPEN_DOCUMENT,		// 開く
//		ACTION_SAVE_DOCUMENT,		// 保存
//		ACTION_SAVE_AS_DOCUMENT,	// 名前を付けて保存
//		ACTION_SAVE_ALL_DOCUMENT,	// 全て保存
//		ACTION_QUIT_APPLICATION,	// 終了
//		ACTION_EDIT_UNDO,			// 元に戻す
//		ACTION_EDIT_REDO,			// やり直し
//		ACTION_EDIT_CUT,			// 切り取り
//		ACTION_EDIT_COPY,			// コピー
//		ACTION_EDIT_PASTE,			// 貼り付け
//		ACTION_EDIT_CLEAR,			// 削除
//		ACTION_SELECT_ALL,			// 全て選択
//		ACTION_FIND_TEXT,			// 検索
//		ACTION_FIND_PREV_TEXT,		// 前を検索
//		ACTION_FIND_NEXT_TEXT,		// 次を検索
//		ACTION_REPLACE_TEXT,		// 置換
//		ACTION_BUILD_SOLUTION,		// ソリューションをビルド
//		ACTION_BUILD_PROJECT,		// プロジェクトをビルド
//		ACTION_COMPILE,				// コンパイル
//		ACTION_EXECUTE,				// 実行
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

	// ソリューション
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
