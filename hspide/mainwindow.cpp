#include "mainwindow.h"
#include "editor.h"
#include "solution.h"
#include "project.h"
#include "outputdock.h"
#include "debuggerdock.h"
#include "projectdock.h"
#include "searchdock.h"
#include "symboldock.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	setObjectName("MainWindow");
	setWindowTitle("HSP script editor");

	CSolution::Config config;
	mSolution = new CSolution(this, config);
	// 起動時は空ソリューション、プロジェクトを作る
	mSolution->append();

	// 処理完了時の通知を登録
	connect(mSolution, SIGNAL(buildStart()),                 this, SLOT(buildStart()));
	connect(mSolution, SIGNAL(buildFinished(bool)),          this, SLOT(buildFinished(bool)));
	connect(mSolution, SIGNAL(buildOutput(const QString &)), this, SLOT(buildOutput(const QString &)));

//	QTextEdit * textEdit = new QTextEdit;
//	setCentralWidget(textEdit);
	//QTabWidget * tabWidget = new QTabWidget(this);
	tabWidget = new QTabWidget(this);
//	QTabBar * tabBar = new QTabBar(this);
//	tabWidget->setTabBar(tabBar);
//	tabWidget->setElideMode(Qt::ElideRight);
	for(int i = 0; i < 5; i++) {
//	tabWidget->addTab(new QTextEdit, "test");
	}
	setCentralWidget(tabWidget);

	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

	setAcceptDrops(true);

	setupDockWindows();
	setupStatusBar();
	setupActions();
	setupToolBars();
	setupMenus();
}

void MainWindow::setupDockWindows()
{
	projectDock = new QDockWidget(tr("Project"), this);
	projectDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	projectDock->setWidget(new CProjectDock(projectDock));
	addDockWidget(Qt::LeftDockWidgetArea, projectDock);

	symbolDock = new QDockWidget(tr("Symbol list"), this);
	symbolDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	symbolDock->setWidget(new CSymbolDock(symbolDock));
	addDockWidget(Qt::LeftDockWidgetArea, symbolDock);

	outputDock = new QDockWidget(tr("Output"), this);
	outputDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	outputDock->setWidget(new COutputDock(outputDock));
	addDockWidget(Qt::BottomDockWidgetArea, outputDock);

	debuggerDock = new QDockWidget(tr("Debugger"), this);
	debuggerDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	debuggerDock->setWidget(new CDebuggerDock(debuggerDock));
	addDockWidget(Qt::BottomDockWidgetArea, debuggerDock);

	searchDock = new QDockWidget(tr("Search result"), this);
	searchDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	searchDock->setWidget(new CSearchDock(searchDock));
	addDockWidget(Qt::BottomDockWidgetArea, searchDock);

	tabifyDockWidget(projectDock, symbolDock);
	tabifyDockWidget(outputDock, debuggerDock);
	tabifyDockWidget(outputDock, searchDock);
}

void MainWindow::setupStatusBar()
{
	QStatusBar * statusBar = this->statusBar();
		QLabel * lable = new QLabel;
		taskProgress = new QProgressBar;
		taskProgress->setRange(0, 100);
		taskProgress->setValue(0);
		taskProgress->setTextVisible(false);
		taskProgress->setVisible(false);
		lable->setMinimumSize( lable->sizeHint() );
		statusBar->addWidget(lable, 1);
		statusBar->addWidget(taskProgress);
	statusBar->showMessage(tr("Ready"));
}

void MainWindow::setupToolBars()
{
	QToolBar * generalToolbar = addToolBar(tr("General"));
		generalToolbar->addAction(newDocumentAct);
		generalToolbar->addAction(openDocumentAct);
		generalToolbar->addAction(saveDocumentAct);
		generalToolbar->addAction(saveAllDocumentAct);
		generalToolbar->addSeparator();
		generalToolbar->addAction(editCutAct);
		generalToolbar->addAction(editCopyAct);
		generalToolbar->addAction(editPasteAct);
		generalToolbar->addSeparator();
		generalToolbar->addAction(editUndoAct);
		generalToolbar->addAction(editRedoAct);
		generalToolbar->addSeparator();
		generalToolbar->addAction(findTextAct);
		generalToolbar->addAction(findPrevTextAct);
		generalToolbar->addAction(findNextTextAct);
		generalToolbar->addAction(replaceTextAct);
		generalToolbar->addSeparator();
		generalToolbar->addAction(debugRunAct);
}

void MainWindow::setupMenus()
{
	QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
		fileMenu->addAction(newDocumentAct);
		fileMenu->addAction(openDocumentAct);
		fileMenu->addSeparator();
		fileMenu->addAction(saveDocumentAct);
		fileMenu->addAction(saveAsDocumentAct);
		fileMenu->addAction(saveAllDocumentAct);
		fileMenu->addSeparator();
		fileMenu->addAction(quitApplicationAct);

	QMenu * editMenu = menuBar()->addMenu(tr("&Edit"));
		editMenu->addAction(editUndoAct);
		editMenu->addAction(editRedoAct);
		editMenu->addSeparator();
		editMenu->addAction(editCutAct);
		editMenu->addAction(editCopyAct);
		editMenu->addAction(editPasteAct);
		editMenu->addAction(editClearAct);
		editMenu->addSeparator();
		editMenu->addAction(selectAllAct);

	QMenu * viewMenu = menuBar()->addMenu(tr("&View"));

	QMenu * searchMenu = menuBar()->addMenu(tr("&Search"));
		searchMenu->addAction(findTextAct);
		searchMenu->addAction(findPrevTextAct);
		searchMenu->addAction(findNextTextAct);
		searchMenu->addAction(replaceTextAct);

	QMenu * buildMenu = menuBar()->addMenu(tr("&Build"));
		buildMenu->addAction(buildProjectAct);
		buildMenu->addAction(buildSolutionAct);
		buildMenu->addSeparator();
		buildMenu->addAction(compileOnlyAct);

	QMenu * debugMenu = menuBar()->addMenu(tr("&Debug"));
		debugMenu->addAction(debugRunAct);
		debugMenu->addAction(noDebugRunAct);

	QMenu * toolsMenu = menuBar()->addMenu(tr("&Tools"));

	QMenu * helpMenu = menuBar()->addMenu(tr("&Help"));
//		debugMenu->addAction(debugRunAct);
}

void MainWindow::setupActions()
{
//	connect(newLetterAct, SIGNAL(triggered()), this, SLOT(newLetter()));

	newDocumentAct = new QAction(QIcon(":/images/tango/document-new.png"), tr("&New"), this);
	newDocumentAct->setShortcuts(QKeySequence::New);
	newDocumentAct->setStatusTip(tr("Create a new file"));
	connect(newDocumentAct, SIGNAL(triggered()), this, SLOT(onNewFile()));

	openDocumentAct = new QAction(QIcon(":/images/tango/document-open.png"), tr("&Open"), this);
	openDocumentAct->setShortcuts(QKeySequence::Open);
	openDocumentAct->setStatusTip(tr("Open file"));
	connect(openDocumentAct, SIGNAL(triggered()), this, SLOT(onOpenFile()));

	saveDocumentAct = new QAction(QIcon(":/images/tango/document-save.png"), tr("&Save"), this);
	saveDocumentAct->setShortcuts(QKeySequence::Save);
	saveDocumentAct->setStatusTip(tr("Save file"));
	connect(saveDocumentAct, SIGNAL(triggered()), this, SLOT(onSaveFile()));

	saveAsDocumentAct = new QAction(QIcon(":/images/tango/document-save-as.png"), tr("S&ave as"), this);
	saveAsDocumentAct->setShortcuts(QKeySequence::SaveAs);
	saveAsDocumentAct->setStatusTip(tr("Save file with a name"));
	connect(saveAsDocumentAct, SIGNAL(triggered()), this, SLOT(onSaveAsFile()));

	saveAllDocumentAct = new QAction(QIcon(":/images/document-save-all.png"), tr("Save all"), this);
	saveAllDocumentAct->setShortcuts(QKeySequence::SaveAs);
	saveAllDocumentAct->setStatusTip(tr("Save all file"));
	connect(saveAllDocumentAct, SIGNAL(triggered()), this, SLOT(onSaveAllFile()));

	quitApplicationAct = new QAction(QIcon(":/images/tango/system-log-out.png"), tr("&Quit"), this);
	quitApplicationAct->setShortcuts(QKeySequence::Quit);
	quitApplicationAct->setStatusTip(tr("Quit application"));
	connect(quitApplicationAct, SIGNAL(triggered()), this, SLOT(onQuit()));

	editUndoAct = new QAction(QIcon(":/images/tango/edit-undo.png"), tr("&Undo"), this);
	editUndoAct->setShortcuts(QKeySequence::Undo);
	editUndoAct->setStatusTip(tr("Undo"));

	editRedoAct = new QAction(QIcon(":/images/tango/edit-redo.png"), tr("&Redo"), this);
	editRedoAct->setShortcuts(QKeySequence::Redo);
	editRedoAct->setStatusTip(tr("Redo"));

	editCutAct = new QAction(QIcon(":/images/tango/edit-cut.png"), tr("Cu&t"), this);
	editCutAct->setShortcuts(QKeySequence::Cut);
	editCutAct->setStatusTip(tr("Cut selected text"));

	editCopyAct = new QAction(QIcon(":/images/tango/edit-copy.png"), tr("&Copy"), this);
	editCopyAct->setShortcuts(QKeySequence::Copy);
	editCopyAct->setStatusTip(tr("Copy selected text"));

	editPasteAct = new QAction(QIcon(":/images/tango/edit-paste.png"), tr("&Paste"), this);
	editPasteAct->setShortcuts(QKeySequence::Paste);
	editPasteAct->setStatusTip(tr("Paste text"));

	editClearAct = new QAction(QIcon(":/images/tango/edit-clear.png"), tr("&Delete"), this);
	editClearAct->setShortcuts(QKeySequence::Delete);
	editClearAct->setStatusTip(tr("Delete selected text"));

	selectAllAct = new QAction(QIcon(":/images/tango/edit-select-all.png"), tr("&Select All"), this);
	selectAllAct->setShortcuts(QKeySequence::SelectAll);
	selectAllAct->setStatusTip(tr("Select all text"));

	findTextAct = new QAction(QIcon(":/images/tango/edit-find.png"), tr("&Find"), this);
	findTextAct->setShortcuts(QKeySequence::Find);
	findTextAct->setStatusTip(tr("Find text"));

	findPrevTextAct = new QAction(QIcon(":/images/tango/edit-select-all.png"), tr("Find &next"), this);
	findPrevTextAct->setShortcuts(QKeySequence::FindNext);
	findPrevTextAct->setStatusTip(tr("Find next text"));

	findNextTextAct = new QAction(QIcon(":/images/tango/edit-select-all.png"), tr("Find &previous"), this);
	findNextTextAct->setShortcuts(QKeySequence::FindPrevious);
	findNextTextAct->setStatusTip(tr("Find previous text"));

	replaceTextAct = new QAction(QIcon(":/images/tango/edit-find-replace.png"), tr("&Replace"), this);
	replaceTextAct->setShortcuts(QKeySequence::Replace);
	replaceTextAct->setStatusTip(tr("Replace text"));


	buildSolutionAct = new QAction(tr("Build &solution"), this);
//	buildSolutionAct->setShortcuts(QKeySequence::Replace);
	buildSolutionAct->setStatusTip(tr("Build solution"));

	buildProjectAct = new QAction(tr("&Build project"), this);
//	buildProjectAct->setShortcuts(QKeySequence::Replace);
	buildProjectAct->setStatusTip(tr("Build project"));

	compileOnlyAct = new QAction(tr("&Compile only"), this);
//	compileOnlyAct->setShortcuts(QKeySequence::Replace);
	compileOnlyAct->setStatusTip(tr("Compile only"));

	debugRunAct = new QAction(QIcon(":/images/tango/media-playback-start.png"), tr("&Debug run"), this);
//	debugRunAct->setShortcuts(QKeySequence::Replace);
	debugRunAct->setStatusTip(tr("Run program with debug"));
	connect(debugRunAct, SIGNAL(triggered()), this, SLOT(onDebugRun()));

	noDebugRunAct = new QAction(tr("&NO debug run"), this);
//	noDebugRunAct->setShortcuts(QKeySequence::Replace);
	noDebugRunAct->setStatusTip(tr("Run program without debug"));

	buildSolutionAct->setEnabled(false);
	buildProjectAct->setEnabled(false);
	compileOnlyAct->setEnabled(false);
	debugRunAct->setEnabled(false);
	noDebugRunAct->setEnabled(false);
}

void MainWindow::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
	if( event->mimeData()->hasUrls() ) {
		event->acceptProposedAction();
	}
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if( event->mimeData()->hasUrls() ) {
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent *event)
{
	const QMimeData *mimeData = event->mimeData();

	if( mimeData->hasUrls() &&
		!mimeData->urls().empty() )
	{
		// 指定数以上の場合、警告メッセージを表示
		// 現状は取り敢えずの実装
		if( 5 < mimeData->urls().size() ) {
			QMessageBox dlg(QMessageBox::Warning, windowTitle(),
							tr(""), 0, this);
			dlg.addButton(tr("&Continue"), QMessageBox::AcceptRole);
			dlg.addButton(tr("C&ancel"),   QMessageBox::RejectRole);
			if( QMessageBox::AcceptRole != dlg.exec() ) {
				return;
			}
		}
		QList<QUrl> urls = mimeData->urls();
		for(int i = 0; i < urls.size(); i++) {
			QString filePath = mimeData->urls().at(i).toLocalFile();
			if( QFileInfo(filePath).exists() ) {
				// 開いている途中に進行状況を出したい
				// ついでに中止ボタンが押せるといいかも
				onOpenFile(filePath);
			}
		}
	}
}

void MainWindow::actionTriggered(QAction *action)
{
	qDebug("action '%s' triggered", action->text().toLocal8Bit().data());
}

void MainWindow::onNewFile()
{
	CEditor * textEditor = new CEditor(tabWidget);
	tabWidget->addTab(textEditor, textEditor->fileName());
	mSolution->openFile(textEditor);
}

void MainWindow::onOpenFile(const QString & filePath)
{
	QString fileName = filePath;

	if( fileName.isEmpty() ) {
		fileName = QFileDialog::getOpenFileName(this,
			tr("Open File"), "", tr("Hot Soup Processor Files (*.hsp *.as)"));
	}

	if( !fileName.isEmpty() ) {
		CEditor * textEditor = new CEditor(tabWidget);
		if( !textEditor->load(fileName) ) {
			delete textEditor;
		} else {
			tabWidget->addTab(textEditor, textEditor->fileName());
			mSolution->openFile(textEditor);
		}
	}
}

void MainWindow::onSaveFile()
{
	CEditor* textEditor
		= dynamic_cast<CEditor*>(tabWidget->currentWidget());

	if( textEditor ) {
		QApplication::setOverrideCursor(Qt::WaitCursor);
		textEditor->save();
		QApplication::restoreOverrideCursor();
	}
}

void MainWindow::onSaveAsFile()
{
	QString fileName
		= QFileDialog::getSaveFileName(this,
			tr("Save File"), "", tr("Hot Soup Processor Files (*.hsp *.as)"));
}

void MainWindow::onSaveAllFile()
{
//	mSolution->save();
}

void MainWindow::onQuit()
{
	qApp->quit();
}

void MainWindow::onDebugRun()
{

//	for(int index = 0, num = tabWidget->count(); index < num; index++)
//	{
//		CEditor * textEdit = static_cast<CEditor*>(tabWidget->widget(index));
//	}

	mSolution->build();
}

void MainWindow::buildStart()
{
	// ビルド処理開始

	dynamic_cast<COutputDock*>(outputDock->widget())->outputCrLf(tr("Build start"));

	// プログラスバーをMarqueeスタイルに
	taskProgress->setRange(0, 0);
	taskProgress->setVisible(true);
}

void MainWindow::buildFinished(bool successed)
{
	// ビルド処理完了

	taskProgress->setVisible(false);

	if( !successed )
	{
	//	QMessageBox::warning(this, windowTitle(), "erro waitForStarted");

		dynamic_cast<COutputDock*>(outputDock->widget())->outputCrLf(tr("Build failed"));
	}
	else
	{
		dynamic_cast<COutputDock*>(outputDock->widget())->outputCrLf(tr("Build complete"));
	}
}

// ビルド中の出力を取得
void MainWindow::buildOutput(const QString & output)
{
	dynamic_cast<COutputDock*>(outputDock->widget())->output(output);
}

void MainWindow::currentTabChanged(int index)
{
	CEditor* textEditor
		= dynamic_cast<CEditor*>(tabWidget->widget(index));

	if( textEditor ) {
		saveDocumentAct->setEnabled( !textEditor->isNoTitle() );
		buildSolutionAct->setEnabled(true);
		buildProjectAct->setEnabled(true);
		compileOnlyAct->setEnabled(true);
		debugRunAct->setEnabled(true);
		noDebugRunAct->setEnabled(true);
	}
}
