#include <QComboBox>
#include "mainwindow.h"
#include "workspaceitem.h"
#include "documentpane.h"
#include "newfiledialog.h"
#include "jumpdialog.h"

QIcon QMultiIcon(const QString & first, const QString & second, const QString & third = QString())
{
	QIcon icon;
	if( !first.isEmpty() ) {
		icon.addFile(first);
	}
	if( !second.isEmpty() ) {
		icon.addFile(second);
	}
	if( !third.isEmpty() ) {
		icon.addFile(third);
	}
	return icon;
}

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
	, m_lastActivatedDocument(NULL)
{
	setObjectName("MainWindow");
	setWindowTitle(tr("HSP script editor"));
	setAcceptDrops(true);
	resize(800, 600);

	m_compiler = new CCompiler(this);

	m_workSpace = new CWorkSpaceModel(this);

	// 処理完了時の通知を登録
	connect(m_compiler, SIGNAL(buildStart(const QString &)),  this, SLOT(buildStart(const QString &)));
	connect(m_compiler, SIGNAL(buildFinished(bool)),          this, SLOT(buildFinished(bool)));
	connect(m_compiler, SIGNAL(buildOutput(const QString &)), this, SLOT(buildOutput(const QString &)));
	connect(m_compiler, SIGNAL(buildOutput(const QString &)), this, SLOT(buildOutput(const QString &)));
	connect(m_compiler, SIGNAL(attachedDebugger(CDebugger*)), this, SLOT(attachedDebugger(CDebugger*)));

	tabWidget = new QTabWidget(this);
	setCentralWidget(tabWidget);

	// タブの一覧と閉じるボタン
	QToolBar * tabButton = new QToolBar(this);
	QAction * tabListAct = tabButton->addAction(QIcon(":/images/button-tab-list.png"), tr("Tab list"));
	QAction * tabCloseAct= tabButton->addAction(QIcon(":/images/button-tab-close.png"), tr("Close tab"));
	tabButton->setIconSize(QSize(12, 12));
	tabButton->setStyleSheet("QToolBar{border:none}");
	tabWidget->setCornerWidget(tabButton, Qt::TopRightCorner);

	setupDockWindows();
	setupStatusBar();
	setupActions();
	setupToolBars();
	setupMenus();

	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));
	connect(projectDock, SIGNAL(oepnItem(CWorkSpaceItem*)), this, SLOT(onOpenFile(CWorkSpaceItem *)));
	connect(tabListAct, SIGNAL(triggered()), this, SLOT(onTabList()));
	connect(tabCloseAct, SIGNAL(triggered()), this, SLOT(onTabClose()));

	loadSettings();

	projectDock->setWorkSpace(m_workSpace);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupDockWindows()
{
	QDockWidget* projectDockWidget = new QDockWidget(tr("Project"), this);
	projectDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	projectDockWidget->setWidget(projectDock = new CProjectDock(projectDockWidget));
	projectDockWidget->setObjectName("Project"); // saveState()で警告がトレースで出るため
	addDockWidget(Qt::LeftDockWidgetArea, projectDockWidget);

	QDockWidget* symbolDockWidget = new QDockWidget(tr("Symbol list"), this);
	symbolDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	symbolDockWidget->setWidget(symbolDock = new CSymbolDock(symbolDockWidget));
	symbolDockWidget->setObjectName("Symbol list"); // saveState()で警告がトレースで出るため
	addDockWidget(Qt::LeftDockWidgetArea, symbolDockWidget);

	QDockWidget* outputDockWidget = new QDockWidget(tr("Output"), this);
	outputDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	outputDockWidget->setWidget(outputDock = new COutputDock(outputDockWidget));
	outputDockWidget->setObjectName("Output"); // saveState()で警告がトレースで出るため
	addDockWidget(Qt::BottomDockWidgetArea, outputDockWidget);

	QDockWidget* debuggerDockWidget = new QDockWidget(tr("Debugger"), this);
	debuggerDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	debuggerDockWidget->setWidget(debuggerDock = new CDebuggerDock(debuggerDockWidget));
	debuggerDockWidget->setObjectName("Debugger"); // saveState()で警告がトレースで出るため
	addDockWidget(Qt::BottomDockWidgetArea, debuggerDockWidget);

	QDockWidget* searchDockWidget = new QDockWidget(tr("Search result"), this);
	searchDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	searchDockWidget->setWidget(searchDock = new CSearchDock(searchDockWidget));
	searchDockWidget->setObjectName("Search result"); // saveState()で警告がトレースで出るため
	addDockWidget(Qt::BottomDockWidgetArea, searchDockWidget);

	QDockWidget* messageDockWidget = new QDockWidget(tr("Messages"), this);
	messageDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	messageDockWidget->setWidget(messageDock = new CMessageDock(messageDockWidget));
	messageDockWidget->setObjectName("Messages"); // saveState()で警告がトレースで出るため
	addDockWidget(Qt::BottomDockWidgetArea, messageDockWidget);

	tabifyDockWidget(projectDockWidget, symbolDockWidget);
	tabifyDockWidget(outputDockWidget, debuggerDockWidget);
	tabifyDockWidget(outputDockWidget, searchDockWidget);
	tabifyDockWidget(outputDockWidget, messageDockWidget);

//	projectDockWidget->setFocus();
	projectDockWidget->raise();

//	outputDockWidget->setFocus();
	outputDockWidget->raise();
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
		generalToolbar->setObjectName("General"); // saveState()で警告がトレースで出るため
		generalToolbar->setIconSize(QSize(16, 16));
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
//		generalToolbar->addWidget(new QComboBox(generalToolbar));
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
		searchMenu->addSeparator();
		searchMenu->addAction(gotoLineAct);

	QMenu * buildMenu = menuBar()->addMenu(tr("&Build"));
		buildMenu->addAction(buildProjectAct);
		buildMenu->addAction(buildSolutionAct);
		buildMenu->addSeparator();
		buildMenu->addAction(compileOnlyAct);

	QMenu * debugMenu = menuBar()->addMenu(tr("&Debug"));
		debugMenu->addAction(debugRunAct);
		debugMenu->addAction(noDebugRunAct);
		debugMenu->addAction(debugAllSuspendAct);
		debugMenu->addAction(debugStopAct);

	QMenu * toolsMenu = menuBar()->addMenu(tr("&Tools"));

	QMenu * helpMenu = menuBar()->addMenu(tr("&Help"));
//		debugMenu->addAction(debugRunAct);
}

void MainWindow::setupActions()
{
//	connect(newLetterAct, SIGNAL(triggered()), this, SLOT(newLetter()));

	newDocumentAct = new QAction(QMultiIcon(":/images/tango/middle/document-new.png",
	                                        ":/images/tango/small/document-new.png"), tr("&New"), this);
	newDocumentAct->setShortcuts(QKeySequence::New);
	newDocumentAct->setStatusTip(tr("Create a new file"));
	connect(newDocumentAct, SIGNAL(triggered()), this, SLOT(onNewFile()));

	openDocumentAct = new QAction(QMultiIcon(":/images/tango/middle/document-open.png",
	                                        ":/images/tango/small/document-open.png"), tr("&Open"), this);
	openDocumentAct->setShortcuts(QKeySequence::Open);
	openDocumentAct->setStatusTip(tr("Open file"));
	connect(openDocumentAct, SIGNAL(triggered()), this, SLOT(onOpenFile()));

	saveDocumentAct = new QAction(QMultiIcon(":/images/tango/middle/document-save.png",
	                                         ":/images/tango/small/document-save.png"), tr("&Save"), this);
	saveDocumentAct->setShortcuts(QKeySequence::Save);
	saveDocumentAct->setStatusTip(tr("Save file"));
	connect(saveDocumentAct, SIGNAL(triggered()), this, SLOT(onSaveFile()));

	saveAsDocumentAct = new QAction(QMultiIcon(":/images/tango/middle/document-save-as.png",
	                                           ":/images/tango/small/document-save-as.png"), tr("S&ave as"), this);
	saveAsDocumentAct->setShortcuts(QKeySequence::SaveAs);
	saveAsDocumentAct->setStatusTip(tr("Save file with a name"));
	connect(saveAsDocumentAct, SIGNAL(triggered()), this, SLOT(onSaveAsFile()));

	saveAllDocumentAct = new QAction(QMultiIcon(":/images/icons/middle/document-save-all.png",
	                                            ":/images/icons/small/document-save-all.png"), tr("Save all"), this);
	saveAllDocumentAct->setShortcuts(QKeySequence::SaveAs);
	saveAllDocumentAct->setStatusTip(tr("Save all file"));
	connect(saveAllDocumentAct, SIGNAL(triggered()), this, SLOT(onSaveAllFile()));

	quitApplicationAct = new QAction(QMultiIcon(":/images/tango/middle/system-log-out.png",
	                                            ":/images/tango/small/system-log-out.png"), tr("&Quit"), this);
	quitApplicationAct->setShortcuts(QKeySequence::Quit);
	quitApplicationAct->setStatusTip(tr("Quit application"));
	connect(quitApplicationAct, SIGNAL(triggered()), this, SLOT(onQuit()));

	editUndoAct = new QAction(QMultiIcon(":/images/tango/middle/edit-undo.png",
	                                     ":/images/tango/small/edit-undo.png"), tr("&Undo"), this);
	editUndoAct->setShortcuts(QKeySequence::Undo);
	editUndoAct->setStatusTip(tr("Undo"));

	editRedoAct = new QAction(QMultiIcon(":/images/tango/middle/edit-redo.png",
	                                     ":/images/tango/small/edit-redo.png"), tr("&Redo"), this);
	editRedoAct->setShortcuts(QKeySequence::Redo);
	editRedoAct->setStatusTip(tr("Redo"));

	editCutAct = new QAction(QMultiIcon(":/images/tango/middle/edit-cut.png",
	                                    ":/images/tango/small/edit-cut.png"), tr("Cu&t"), this);
	editCutAct->setShortcuts(QKeySequence::Cut);
	editCutAct->setStatusTip(tr("Cut selected text"));

	editCopyAct = new QAction(QMultiIcon(":/images/tango/middle/edit-copy.png",
	                                     ":/images/tango/small/edit-copy.png"), tr("&Copy"), this);
	editCopyAct->setShortcuts(QKeySequence::Copy);
	editCopyAct->setStatusTip(tr("Copy selected text"));

	editPasteAct = new QAction(QMultiIcon(":/images/tango/middle/edit-paste.png",
	                                      ":/images/tango/small/edit-paste.png"), tr("&Paste"), this);
	editPasteAct->setShortcuts(QKeySequence::Paste);
	editPasteAct->setStatusTip(tr("Paste text"));

	editClearAct = new QAction(QMultiIcon(":/images/tango/middle/edit-clear.png",
	                                      ":/images/tango/small/edit-clear.png"), tr("&Delete"), this);
	editClearAct->setShortcuts(QKeySequence::Delete);
	editClearAct->setStatusTip(tr("Delete selected text"));

	selectAllAct = new QAction(QMultiIcon(":/images/tango/middle/edit-select-all.png",
	                                      ":/images/tango/small/edit-select-all.png"), tr("&Select All"), this);
	selectAllAct->setShortcuts(QKeySequence::SelectAll);
	selectAllAct->setStatusTip(tr("Select all text"));

	findTextAct = new QAction(QMultiIcon(":/images/tango/middle/edit-find.png",
	                                     ":/images/tango/small/edit-find.png"), tr("&Find"), this);
	findTextAct->setShortcuts(QKeySequence::Find);
	findTextAct->setStatusTip(tr("Find text"));

	findPrevTextAct = new QAction(QMultiIcon(":/images/tango/middle/edit-select-all.png",
	                                         ":/images/tango/small/edit-select-all.png"), tr("Find &next"), this);
	findPrevTextAct->setShortcuts(QKeySequence::FindNext);
	findPrevTextAct->setStatusTip(tr("Find next text"));

	findNextTextAct = new QAction(QMultiIcon(":/images/tango/middle/edit-select-all.png",
	                                         ":/images/tango/small/edit-select-all.png"), tr("Find &previous"), this);
	findNextTextAct->setShortcuts(QKeySequence::FindPrevious);
	findNextTextAct->setStatusTip(tr("Find previous text"));

	replaceTextAct = new QAction(QMultiIcon(":/images/tango/middle/edit-find-replace.png",
	                                        ":/images/tango/small/edit-find-replace.png"), tr("&Replace"), this);
	replaceTextAct->setShortcuts(QKeySequence::Replace);
	replaceTextAct->setStatusTip(tr("Replace text"));

	gotoLineAct = new QAction(QMultiIcon(":/images/tango/middle/go-jump.png",
	                                     ":/images/tango/small/go-jump.png"), tr("&Go to line"), this);
	gotoLineAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_J));
	gotoLineAct->setStatusTip(tr("Go to line"));
	connect(gotoLineAct, SIGNAL(triggered()), this, SLOT(onGoToLine()));


	buildSolutionAct = new QAction(tr("Build &solution"), this);
//	buildSolutionAct->setShortcuts(QKeySequence::Replace);
	buildSolutionAct->setStatusTip(tr("Build solution"));
	connect(buildSolutionAct, SIGNAL(triggered()), this, SLOT(onBuildSolution()));

	buildProjectAct = new QAction(tr("&Build project"), this);
//	buildProjectAct->setShortcuts(QKeySequence::Replace);
	buildProjectAct->setStatusTip(tr("Build project"));
	connect(buildProjectAct, SIGNAL(triggered()), this, SLOT(onBuildProject()));

	compileOnlyAct = new QAction(tr("&Compile only"), this);
//	compileOnlyAct->setShortcuts(QKeySequence::Replace);
	compileOnlyAct->setStatusTip(tr("Compile only"));

	debugRunAct = new QAction(QMultiIcon(":/images/tango/middle/media-playback-start.png",
	                                     ":/images/tango/small/media-playback-start.png"), tr("&Debug run"), this);
//	debugRunAct->setShortcuts(QKeySequence::Replace);
	debugRunAct->setStatusTip(tr("Run program with debug"));
	connect(debugRunAct, SIGNAL(triggered()), this, SLOT(onDebugRun()));

	noDebugRunAct = new QAction(tr("&NO debug run"), this);
//	noDebugRunAct->setShortcuts(QKeySequence::Replace);
	noDebugRunAct->setStatusTip(tr("Run program without debug"));
	connect(noDebugRunAct, SIGNAL(triggered()), this, SLOT(onNoDebugRun()));

	debugAllSuspendAct = new QAction(QMultiIcon(":/images/tango/middle/media-playback-pause.png",
	                                            ":/images/tango/small/media-playback-pause.png"), tr("&All suspend"), this);
//	debugAllSuspendAct->setShortcuts(QKeySequence::Replace);
	debugAllSuspendAct->setStatusTip(tr("Run program without debug"));
	connect(debugAllSuspendAct, SIGNAL(triggered()), this, SLOT(onDebugAllSuspend()));

	debugStopAct = new QAction(QMultiIcon(":/images/tango/middle/media-playback-stop.png",
	                                      ":/images/tango/small/media-playback-stop.png"), tr("&Stop debugging"), this);
//	debugStopAct->setShortcuts(QKeySequence::Replace);
	debugStopAct->setStatusTip(tr("Run program without debug"));
	connect(debugStopAct, SIGNAL(triggered()), this, SLOT(onDebugStop()));

	saveDocumentAct->setEnabled(false);
	saveAsDocumentAct->setEnabled(false);
	saveAllDocumentAct->setEnabled(false);
	editUndoAct->setEnabled(false);
	editRedoAct->setEnabled(false);
	editCutAct->setEnabled(false);
	editCopyAct->setEnabled(false);
	editPasteAct->setEnabled(false);
	editClearAct->setEnabled(false);
	selectAllAct->setEnabled(false);
	buildSolutionAct->setEnabled(false);
	buildProjectAct->setEnabled(false);
	compileOnlyAct->setEnabled(false);
//	debugRunAct->setEnabled(false);
//	noDebugRunAct->setEnabled(false);
}

void MainWindow::loadSettings()
{
//	QSettings settings("sharkpp", "hspide");
	QSettings settings("hspide.ini", QSettings::IniFormat);
	settings.setIniCodec("UTF-8"); // 文字コードを指定

	QVariant tmp;

	// コンパイラパスを取得
	m_compiler->setCompilerPath(settings.value("path/compiler", QDir::currentPath()).toString());
	// HSPディレクトリの取得
	m_compiler->setHspPath(settings.value("path/hsp", QDir::currentPath()).toString());
	// Commonディレクトリの取得
	m_compiler->setHspCommonPath(settings.value("path/hsp-common", m_compiler->hspPath() + "/common/").toString());

	// ウインドウ位置などを取得
	restoreGeometry(settings.value("window/geometry").toByteArray());
	restoreState(settings.value("window/state").toByteArray());
}

void MainWindow::saveSettings()
{
//	QSettings settings("sharkpp", "hspide");
	QSettings settings("hspide.ini", QSettings::IniFormat);
	settings.setIniCodec("UTF-8"); // 文字コードを指定

	QString tmp;

	// コンパイラパスを保存
	settings.setValue("path/compiler", m_compiler->compilerPath());
	// HSPディレクトリの保存
	settings.setValue("path/hsp", m_compiler->hspPath());
	// Commonディレクトリの保存
	settings.setValue("path/hsp-common", m_compiler->hspCommonPath());

	// ウインドウ位置などを保存
	settings.setValue("window/geometry", saveGeometry());
	settings.setValue("window/state", saveState());
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

void MainWindow::closeEvent(QCloseEvent *event)
{
	saveSettings();
	QMainWindow::closeEvent(event);
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
	CNewFileDialog dlg(this);
	switch( dlg.exec() )
	{
	case QDialog::Accepted:
		// ファイル名が無いファイルの場合はあとでファイル保存ダイアログを出すで登録
		break;
	default:
		return;
	}
	// ソリューションにプロジェクトを追加し、
	// タブで追加したファイルを開く
	CWorkSpaceItem* projectItem = m_workSpace->appendProject();
	CDocumentPane * document    = new CDocumentPane(tabWidget);
	document->setSymbols(m_compiler->symbols());
	document->setAssignItem(projectItem);
	document->load(dlg.filePath(), dlg.templateFilePath());
	tabWidget->addTab(document, document->fileName());
	tabWidget->setCurrentWidget(document);
}

void MainWindow::onOpenFile()
{
	onOpenFile(QString());
}

void MainWindow::onOpenFile(const QString & filePath)
{
	QString fileName = filePath;

	// ファイル名が指定されていなかったら開く
	if( fileName.isEmpty() ) {
		fileName = QFileDialog::getOpenFileName(this,
			tr("Open File"), "",
			tr("All Supported Files (*.hsp *.as *.hspsln)") + ";;" +
			tr("Hot Soup Processor Files (*.hsp *.as)") + ";;" +
			tr("HSP IDE Solution File (*.hspsln)") + ";;" +
			tr("All File (*.*)")
			);
	}

	if( !fileName.isEmpty() ) {
		QString ext = QFileInfo(fileName).suffix();
		if( !ext.compare("hspsln", Qt::CaseSensitive) ) {
			CWorkSpaceItem * solutionItem = projectDock->currentSolution();
			if( solutionItem &&
				solutionItem->save() &&
				solutionItem->load(fileName) )
			{
			}
		} else {
			CWorkSpaceItem * parentItem = projectDock->currentProject();
			CWorkSpaceItem * fileItem   = m_workSpace->appendFile(fileName, parentItem);
			onOpenFile(fileItem);
		}
	}
}

void MainWindow::onOpenFile(CWorkSpaceItem * item)
{
	// すでに開いていたらタブを選択しなおす
	for(int index = 0; index < tabWidget->count(); index++)
	{
		CDocumentPane* document
			= dynamic_cast<CDocumentPane*>(tabWidget->widget(index));
		if( document &&
			item == document->assignItem() )
		{
			tabWidget->setCurrentIndex(index);
			return;
		}
	}

	// ダブルクリックで開けるのはファイルのみ
	if( CWorkSpaceItem::FileNode != item->nodeType() )
	{
		return;
	}

	// 新たに開く
	CDocumentPane * document = new CDocumentPane(tabWidget);
	document->setSymbols(m_compiler->symbols());
	document->setAssignItem(item);
	document->load(item->path());
	tabWidget->addTab(document, document->fileName());
	tabWidget->setCurrentWidget(document);
}

void MainWindow::onSaveFile()
{
	CWorkSpaceItem* item = projectDock->currentItem();

	QApplication::setOverrideCursor(Qt::WaitCursor);

	if( item &&
		item->save() )
	{
		if( CDocumentPane* document = item->assignDocument() )
		{
			// ファイル名を更新
			tabWidget->setTabText(tabWidget->currentIndex(), document->fileName());
		}
	}

	QApplication::restoreOverrideCursor();
}

void MainWindow::onSaveAsFile()
{
	QString fileName
		= QFileDialog::getSaveFileName(this,
			tr("Save File"), "", tr("Hot Soup Processor Files (*.hsp *.as)"));
}

void MainWindow::onSaveAllFile()
{
//	m_workSpace->save();
}

void MainWindow::onQuit()
{
	qApp->quit();
}

void MainWindow::onGoToLine()
{
	CJumpDialog dlg(this);

	if( QDialog::Accepted == dlg.exec() )
	{
		// 行を移動
		CDocumentPane* document
			= dynamic_cast<CDocumentPane*>(tabWidget->currentWidget());
		if( document )
		{
			document->jump(dlg.lineNo());
		}
	}
}

void MainWindow::onBuildProject()
{
}

void MainWindow::onBuildSolution()
{
}

void MainWindow::onDebugRun()
{
	CDocumentPane * document = static_cast<CDocumentPane*>(tabWidget->currentWidget());
	CWorkSpaceItem* currentItem = NULL;

	if( document )
	{
		// 現在選択しているタブに関連付けられているファイルを取得
		currentItem = document->assignItem();
	}
	else
	{
		// なければ、プロジェクトツリーから選択しているファイルを取得
		currentItem = projectDock->currentItem();
	}

	// もし、プロジェクトに属していたら取得
	CWorkSpaceItem* currentProject
		= currentItem->ancestor(CWorkSpaceItem::Project);

	// もし、ソリューションに属していたら取得
	CWorkSpaceItem* currentSolution
		= currentItem->ancestor(CWorkSpaceItem::Solution);

	if( currentSolution &&
		0 )
	{
		// ソリューションが取得できたらソリューション構成を取得しビルド＆実行
		
	}
	else if( currentProject )
	{
		// プロジェクトが取得できたらビルド＆実行
		m_compiler->run(currentProject, true);
	}
}

void MainWindow::onNoDebugRun()
{
	CDocumentPane * document = static_cast<CDocumentPane*>(tabWidget->currentWidget());
	CWorkSpaceItem* currentItem = NULL;

	if( document )
	{
		// 現在選択しているタブに関連付けられているファイルを取得
		currentItem = document->assignItem();
	}
	else
	{
		// なければ、プロジェクトツリーから選択しているファイルを取得
		currentItem = projectDock->currentItem();
	}

	// もし、プロジェクトに属していたら取得
	CWorkSpaceItem* currentProject
		= currentItem->ancestor(CWorkSpaceItem::Project);

	// もし、ソリューションに属していたら取得
	CWorkSpaceItem* currentSolution
		= currentItem->ancestor(CWorkSpaceItem::Solution);

	if( currentSolution &&
		0 )
	{
		// ソリューションが取得できたらソリューション構成を取得しビルド＆実行
		
	}
	else if( currentProject )
	{
		// プロジェクトが取得できたらビルド＆実行
		m_compiler->run(currentProject, false);
	}
}

void MainWindow::onDebugAllSuspend()
{
	// 全てのデバッグを中断
	foreach(CDebugger* debugger, m_debuggers) {
		debugger->suspendDebugging();
	}
}

void MainWindow::onDebugStop()
{
	// 全てのデバッグを停止
	foreach(CDebugger* debugger, m_debuggers) {
		debugger->stopDebugging();
	}
}

void MainWindow::onTabList()
{
}

// タブを閉じる
void MainWindow::onTabClose()
{
	// 何はともあれ保存
	onSaveFile();

	// 関連付けを解除
	CDocumentPane * document = static_cast<CDocumentPane*>(tabWidget->currentWidget());

	CWorkSpaceItem * solutionItem = projectDock->currentSolution();
	CWorkSpaceItem * currentItem  = document->assignItem();

	currentItem->setAssignDocument(NULL);

	// ソリューションが空もしくはプロジェクトに名前を
	// つけていない場合はツリーから削除
	if( (solutionItem && solutionItem->isUntitled()) ||
		currentItem->isUntitled() )
	{
		m_workSpace->remove(currentItem);
	}

	// タブを閉じる
	tabWidget->removeTab(tabWidget->currentIndex());
}

void MainWindow::buildStart(const QString & filePath)
{
	// ビルド処理開始

	outputDock->outputCrLf(tr("Build start"));

	CWorkSpaceItem* targetProjectItem
		= projectDock->currentSolution()->search(filePath);

	// ビルドを開始したプロジェクトに関連するファイルを全て保存
	if( targetProjectItem )
	{
		// ファイルを保存
		for(int index = 0, num = tabWidget->count(); index < num; index++)
		{
			CDocumentPane * textEdit = dynamic_cast<CDocumentPane*>(tabWidget->widget(index));
			CWorkSpaceItem* assignItem = textEdit->assignItem();
			if( targetProjectItem ==
					assignItem->ancestor(CWorkSpaceItem::Project) )
			{
				if( !textEdit->isUntitled() )
				{
					textEdit->save();
				}
			}
		}
	}

	// プログラスバーをMarqueeスタイルに
	taskProgress->setRange(0, 0);
	taskProgress->setVisible(true);

	// 出力をクリア
	outputDock->clear();
	messageDock->clear();
}

void MainWindow::buildFinished(bool successed)
{
	// ビルド処理完了

	taskProgress->setVisible(false);

	if( !successed )
	{
	//	QMessageBox::warning(this, windowTitle(), "erro waitForStarted");

		outputDock->outputCrLf(tr("Build failed"));
	}
	else
	{
		outputDock->outputCrLf(tr("Build complete"));
	}
}

// ビルド中の出力を取得
void MainWindow::buildOutput(const QString & output)
{
	QString tmp = QString(output).replace("\r\n", "\n");
	outputDock->output(tmp);

	QRegExp reCompiler("^((.+)\\(([0-9]+)\\) : (.+))$");
	QRegExp rePreProcessor("^#Error:(.+) in line ([0-9]+) \\[(.*)\\]$");

	for(QStringListIterator ite(tmp.split("\n"));
		ite.hasNext(); )
	{
		QString line = ite.next();
		line.replace("\r", "");

		if( 0 <= reCompiler.indexIn(line) ) {
			QString fileName = reCompiler.cap(2);
			QString lineNum  = reCompiler.cap(3);
			QString desc     = reCompiler.cap(4);
			messageDock->addMessage(fileName, lineNum.toInt(), desc);
		}
		else if( 0 <= rePreProcessor.indexIn(line) ) {
			QString fileName = rePreProcessor.cap(3);
			QString lineNum  = rePreProcessor.cap(2);
			QString desc     = rePreProcessor.cap(1);
			messageDock->addMessage(fileName, lineNum.toInt(), desc);
		}
	}
}

void MainWindow::attachedDebugger(CDebugger* debugger)
{
	connect(debugger, SIGNAL(stopAtBreakPoint(const QUuid &,int)), this, SLOT(stopAtBreakPoint(const QUuid &,int)));
	connect(debugger, SIGNAL(destroyed()), this, SLOT(dettachedDebugger()));

	m_debuggers.insert(debugger);
}

void MainWindow::dettachedDebugger()
{
	CDebugger* debugger = reinterpret_cast<CDebugger*>(sender());

	m_debuggers.remove(debugger);
}

void MainWindow::stopAtBreakPoint(const QUuid & uuid, int lineNum)
{
	CWorkSpaceItem* targetProjectItem
		= projectDock->currentSolution()->search(uuid);

	if( targetProjectItem )
	{
		// ファイルを開く
		onOpenFile(targetProjectItem);

		// 行を移動
		CDocumentPane* document
			= dynamic_cast<CDocumentPane*>(tabWidget->currentWidget());
		if( document )
		{
			document->jump(lineNum);
		}

		activateWindow();
	}
}

// 編集された
void MainWindow::onDocumentChanged()
{
	CDocumentPane* document
		= dynamic_cast<CDocumentPane*>(tabWidget->currentWidget());

	if( document )
	{
	//	saveDocumentAct->setEnabled( document->isModified() );
		buildSolutionAct->setEnabled(true);
		buildProjectAct->setEnabled(true);
		compileOnlyAct->setEnabled(true);
//		debugRunAct->setEnabled(true);
//		noDebugRunAct->setEnabled(true);
	}
}

// タブが変更された
void MainWindow::currentTabChanged(int index)
{
	CDocumentPane* document
		= dynamic_cast<CDocumentPane*>(tabWidget->widget(index));

	if( m_lastActivatedDocument )
	{
		CDocumentPane* lastDocument
			= dynamic_cast<CDocumentPane*>(m_lastActivatedDocument);

		// シグナルとの関連付けを切断
		disconnect(lastDocument,                       SIGNAL(modificationChanged(bool)));
		disconnect(lastDocument->editor()->document(), SIGNAL(undoAvailable(bool)));
		disconnect(lastDocument->editor()->document(), SIGNAL(redoAvailable(bool)));
		disconnect(lastDocument->editor(),             SIGNAL(copyAvailable(bool)));
		disconnect(lastDocument->editor(),             SIGNAL(copyAvailable(bool)));
	}

	if( document )
	{
		// シグナルと関連付け
		connect(document,                       SIGNAL(modificationChanged(bool)), saveDocumentAct, SLOT(setEnabled(bool)));
		connect(document->editor()->document(), SIGNAL(undoAvailable(bool)),       editUndoAct,     SLOT(setEnabled(bool)));
		connect(document->editor()->document(), SIGNAL(redoAvailable(bool)),       editRedoAct,     SLOT(setEnabled(bool)));
		connect(document->editor(),             SIGNAL(copyAvailable(bool)),       editCutAct,      SLOT(setEnabled(bool)));
		connect(document->editor(),             SIGNAL(copyAvailable(bool)),       editCopyAct,     SLOT(setEnabled(bool)));

		document->setFocus();

		projectDock->selectItem(document->assignItem());
	}

	saveDocumentAct->   setEnabled( document ? document->isModified() : false );
	saveAsDocumentAct-> setEnabled( NULL != document );
	saveAllDocumentAct->setEnabled( NULL != document );

	m_lastActivatedDocument = document;

	onDocumentChanged();
}

