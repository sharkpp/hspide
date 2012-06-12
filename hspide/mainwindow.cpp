#include "mainwindow.h"
#include "workspaceitem.h"
#include "documentpane.h"
#include "newfiledialog.h"
#include "jumpdialog.h"
#include "configdialog.h"
#include "savesolutiondialog.h"
#include "ui_aboutdialog.h"

Configuration     theConf;
FileManager       theFile;
BreakPointManager theBreakPoint;

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
	setWindowIcon(QIcon(":/images/icons/hspide.png"));
	resize(800, 600);

	m_compiler = new CCompiler(this);

	m_workSpace = new CWorkSpaceModel(this);
//	m_workSpace->setConfiguration(m_configuration);

	// 処理完了時の通知を登録
	connect(m_compiler, SIGNAL(buildStart(int, const QString &)),  this, SLOT(buildStart(int, const QString &)));
	connect(m_compiler, SIGNAL(buildFinished(int, bool)),          this, SLOT(buildFinished(int, bool)));
	connect(m_compiler, SIGNAL(buildOutput(int, const QString &)), this, SLOT(buildOutput(int, const QString &)));
	connect(m_compiler, SIGNAL(attachedDebugger(CDebugger*)),      this, SLOT(attachedDebugger(CDebugger*)));
	connect(m_compiler, SIGNAL(updatedSymbols()),                  this, SLOT(onUpdatedSymbols()));

	tabWidget = new QTabWidget(this);
	setCentralWidget(tabWidget);

	// タブの一覧と閉じるボタン
	QToolBar * tabButton = new QToolBar(this);
	QAction * tabListAct = tabButton->addAction(QIcon(":/images/button-tab-list.png"), tr("Tab list"));
	QAction * tabCloseAct= tabButton->addAction(QIcon(":/images/button-tab-close.png"), tr("Close tab"));
	tabButton->setIconSize(QSize(12, 12));
	tabButton->setStyleSheet("QToolBar{border:none}");
	tabWidget->setCornerWidget(tabButton, Qt::TopRightCorner);

	m_tabListMenu = new QMenu(this);
	connect(m_tabListMenu, SIGNAL(aboutToShow()), this, SLOT(onTabListShow()));

	setupDockWindows();
	setupStatusBar();
	setupActions();
	setupToolBars();
	setupMenus();

	connect(tabWidget,     SIGNAL(currentChanged(int)),                        this, SLOT(currentTabChanged(int)));
	connect(projectDock,   SIGNAL(oepnItem(CWorkSpaceItem*)),                  this, SLOT(onOpenFile(CWorkSpaceItem *)));
	connect(tabListAct,    SIGNAL(triggered()),                                this, SLOT(onTabList()));
	connect(tabCloseAct,   SIGNAL(triggered()),                                this, SLOT(onTabClose()));
	connect(messageDock,   SIGNAL(gotoLine(const QUuid &, int)),               this, SLOT(onGoToLine(const QUuid &, int)));
	connect(symbolDock,    SIGNAL(gotoLine(const QUuid &, int)),               this, SLOT(onGoToLine(const QUuid &, int)));
	connect(breakPointDock,SIGNAL(gotoLine(const QUuid &, int)),               this, SLOT(onGoToLine(const QUuid &, int)));
	connect(varInfoDock,   SIGNAL(requestVariableInfo(const QString& , int*)), this, SLOT(onReqVarInfo(const QString& , int*)));
	connect(qobject_cast<QDockWidget*>(symbolDock->parentWidget()),
	                       SIGNAL(visibilityChanged(bool)),                    this, SLOT(onSymbolDockVisibilityChanged(bool)));
	connect(&theConf,      SIGNAL(updateConfiguration(const Configuration&)),  this, SLOT(updateConfiguration(const Configuration&)));

	loadSettings();

	projectDock->setWorkSpace(m_workSpace);

	// とりあえず空のファイルを追加
	onOpenFile(m_workSpace->appendProject());
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

	QDockWidget* symbolDockWidget = new QDockWidget(tr("Symbols"), this);
	symbolDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	symbolDockWidget->setWidget(symbolDock = new CSymbolDock(symbolDockWidget));
	symbolDockWidget->setObjectName("Symbols"); // saveState()で警告がトレースで出るため
	addDockWidget(Qt::LeftDockWidgetArea, symbolDockWidget);

	QDockWidget* outputDockWidget = new QDockWidget(tr("Output"), this);
	outputDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	outputDockWidget->setWidget(outputDock = new COutputDock(outputDockWidget));
	outputDockWidget->setObjectName("Output"); // saveState()で警告がトレースで出るため
	addDockWidget(Qt::BottomDockWidgetArea, outputDockWidget);

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

	QDockWidget* breakPointDockWidget = new QDockWidget(tr("Breakpoints"), this);
	breakPointDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	breakPointDockWidget->setWidget(breakPointDock = new CBreakPointDock(breakPointDockWidget));
	breakPointDockWidget->setObjectName("Breakpoints"); // saveState()で警告がトレースで出るため
	breakPointDockWidget->setVisible(false);

	// ドックタブに結合
	tabifyDockWidget(projectDockWidget, symbolDockWidget);
	tabifyDockWidget(outputDockWidget, searchDockWidget);
	tabifyDockWidget(outputDockWidget, messageDockWidget);
	tabifyDockWidget(outputDockWidget, breakPointDockWidget);

//	projectDockWidget->setFocus();
	projectDockWidget->raise();

//	outputDockWidget->setFocus();
	outputDockWidget->raise();

 // デバッグ時のみ有効なドック

	QDockWidget* sysInfoDockWidget = new QDockWidget(tr("System variables"), this);
	sysInfoDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	sysInfoDockWidget->setWidget(sysInfoDock = new CSystemInfoDock(sysInfoDockWidget));
	sysInfoDockWidget->setObjectName("System variables"); // saveState()で警告がトレースで出るため
	sysInfoDockWidget->setVisible(false);

	QDockWidget* varInfoDockWidget = new QDockWidget(tr("Variables"), this);
	varInfoDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	varInfoDockWidget->setWidget(varInfoDock = new CVariableInfoDock(varInfoDockWidget));
	varInfoDockWidget->setObjectName("Variables"); // saveState()で警告がトレースで出るため
	varInfoDockWidget->setVisible(false);
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
	statusBar->setStyleSheet(
			"QStatusBar {"
				"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
				                            "stop: 0   #999999,"
				                            "stop: 0.1 #CCCCCC,"
				                            "stop: 0.2 transparent,"
				                            "stop: 1.0 transparent);"
			"}"
		);
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
		generalToolbar->addAction(debugRunSolutionAct);
		generalToolbar->addAction(debugResumeAct);
		generalToolbar->addAction(debugSuspendAct);
		generalToolbar->addAction(debugStopAct);
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
		viewMenu->addAction(showProjectDockAct);
		viewMenu->addAction(showSymbolDockAct);
		viewMenu->addAction(showOutputDockAct);
		viewMenu->addAction(showSearchDockAct);
		viewMenu->addAction(showMessageDockAct);
		viewMenu->addAction(showBreakPointDockAct);

	QMenu * searchMenu = menuBar()->addMenu(tr("&Search"));
		searchMenu->addAction(findTextAct);
		searchMenu->addAction(findPrevTextAct);
		searchMenu->addAction(findNextTextAct);
		searchMenu->addAction(replaceTextAct);
		searchMenu->addSeparator();
		searchMenu->addAction(gotoLineAct);

	QMenu * buildMenu = menuBar()->addMenu(tr("&Build"));
		buildMenu->addAction(buildSolutionAct);
		buildMenu->addAction(buildProjectAct);
		buildMenu->addAction(batchBuildAct);

	QMenu * debugMenu = menuBar()->addMenu(tr("&Debug"));
		debugMenu->addAction(debugRunSolutionAct);
		debugMenu->addAction(noDebugRunSolutionAct);
		debugMenu->addSeparator();
		debugMenu->addAction(debugRunProjectAct);
		debugMenu->addAction(noDebugRunProjectAct);
		debugMenu->addSeparator();
		debugMenu->addAction(debugResumeAct);
		debugMenu->addAction(debugSuspendAct);
		debugMenu->addAction(debugStopAct);

	QMenu * toolsMenu = menuBar()->addMenu(tr("&Tools"));
		toolsMenu->addSeparator();
		toolsMenu->addAction(settingAct);

	QMenu * helpMenu = menuBar()->addMenu(tr("&Help"));
		helpMenu->addSeparator();
		helpMenu->addAction(aboutAct);
}

void MainWindow::setupActions()
{
//	connect(newLetterAct, SIGNAL(triggered()), this, SLOT(newLetter()));

	struct {
		QAction** action;
		QString   middleIcon, smallIcon;
		QString   text, tooltipText, statusText;
	} menuInitTable[] = {
		{ &newDocumentAct,        ":/images/tango/middle/document-new.png",         ":/images/tango/small/document-new.png",          tr("&New"),                  tr("New"),                  tr("Create a new file")         },
		{ &openDocumentAct,       ":/images/tango/middle/document-open.png",        ":/images/tango/small/document-open.png",         tr("&Open"),                 tr("Open"),                 tr("Open file")                 },
		{ &saveDocumentAct,       ":/images/tango/middle/document-save.png",        ":/images/tango/small/document-save.png",         tr("&Save"),                 tr("Save"),                 tr("Save file")                 },
		{ &saveAsDocumentAct,     ":/images/tango/middle/document-save-as.png",     ":/images/tango/small/document-save-as.png",      tr("S&ave as"),              tr("Save as"),              tr("Save file with a name")     },
		{ &saveAllDocumentAct,    ":/images/icons/middle/document-save-all.png",    ":/images/icons/small/document-save-all.png",     tr("Save all"),              tr("Save all"),             tr("Save all file")             },
		{ &quitApplicationAct,    ":/images/tango/middle/system-log-out.png",       ":/images/tango/small/system-log-out.png",        tr("&Quit"),                 tr("Quit"),                 tr("Quit application")          },
		{ &editUndoAct,           ":/images/tango/middle/edit-undo.png",            ":/images/tango/small/edit-undo.png",             tr("&Undo"),                 tr("Undo"),                 tr("Undo")                      },
		{ &editRedoAct,           ":/images/tango/middle/edit-redo.png",            ":/images/tango/small/edit-redo.png",             tr("&Redo"),                 tr("Redo"),                 tr("Redo")                      },
		{ &editCutAct,            ":/images/tango/middle/edit-cut.png",             ":/images/tango/small/edit-cut.png",              tr("Cu&t"),                  tr("Cut"),                  tr("Cut selected text")         },
		{ &editCopyAct,           ":/images/tango/middle/edit-copy.png",            ":/images/tango/small/edit-copy.png",             tr("&Copy"),                 tr("Copy"),                 tr("Copy selected text")        },
		{ &editPasteAct,          ":/images/tango/middle/edit-paste.png",           ":/images/tango/small/edit-paste.png",            tr("&Paste"),                tr("Paste"),                tr("Paste text")                },
		{ &editClearAct,          ":/images/tango/middle/edit-clear.png",           ":/images/tango/small/edit-clear.png",            tr("&Delete"),               tr("Delete"),               tr("Delete selected text")      },
		{ &selectAllAct,          ":/images/tango/middle/edit-select-all.png",      ":/images/tango/small/edit-select-all.png",       tr("&Select All"),           tr("Select All"),           tr("Select all text")           },
		{ &findTextAct,           ":/images/tango/middle/edit-find.png",            ":/images/tango/small/edit-find.png",             tr("&Find"),                 tr("Find"),                 tr("Find text")                 },
		{ &findPrevTextAct,       ":/images/tango/middle/edit-select-all.png",      ":/images/tango/small/edit-select-all.png",       tr("Find &next"),            tr("Find next"),            tr("Find next text")            },
		{ &findNextTextAct,       ":/images/tango/middle/edit-select-all.png",      ":/images/tango/small/edit-select-all.png",       tr("Find &previous"),        tr("Find previous"),        tr("Find previous text")        },
		{ &replaceTextAct,        ":/images/tango/middle/edit-find-replace.png",    ":/images/tango/small/edit-find-replace.png",     tr("&Replace"),              tr("Replace"),              tr("Replace text")              },
		{ &gotoLineAct,           ":/images/tango/middle/go-jump.png",              ":/images/tango/small/go-jump.png",               tr("&Go to line"),           tr("Go to line"),           tr("Go to line")                },
		{ &showProjectDockAct,    "",                                               "",                                               tr("&Project"),              tr("Show project"),         tr("Show project")              },
		{ &showSymbolDockAct,     "",                                               "",                                               tr("S&ymbols"),              tr("Show symbols"),         tr("Show symbols")              },
		{ &showOutputDockAct,     "",                                               "",                                               tr("&Output"),               tr("Show output"),          tr("Show output")               },
		{ &showSearchDockAct,     "",                                               "",                                               tr("&Search"),               tr("Show search"),          tr("Show search")               },
		{ &showMessageDockAct,    "",                                               "",                                               tr("&Messages"),             tr("Show messages"),        tr("Show messages")             },
		{ &showBreakPointDockAct, "",                                            "",                                                  tr("&Breakpoints"),          tr("Show breakpoints"),     tr("Show breakpoints")          },
		{ &showSysInfoDockAct,    "",                                               "",                                               tr("&System variables"),     tr("Show system variables"),tr("Show system variables")     },
		{ &showVarInfoDockAct,    "",                                               "",                                               tr("&Variables"),            tr("Show variables"),       tr("Show variables")            },
		{ &buildSolutionAct,      "",                                               "",                                               tr("Build &solution"),       tr("Build solution"),       tr("Build solution")            },
		{ &buildProjectAct,       "",                                               "",                                               tr("&Build project"),        tr("Build project"),        tr("Build project")             },
		{ &batchBuildAct,         "",                                               "",                                               tr("B&atch build"),          tr("Batch build"),          tr("Batch build")               },
		{ &compileOnlyAct,        "",                                               "",                                               tr("&Compile only"),         tr("Compile only"),         tr("Compile only")              },
		{ &debugRunSolutionAct,   ":/images/tango/middle/media-playback-start.png", ":/images/tango/small/media-playback-start.png",  tr("&Debug run solution"),   tr("Debug run solution"),   tr("Run solution with debug")   },
		{ &noDebugRunSolutionAct, "",                                               "",                                               tr("&NO debug run solution"),tr("NO debug run solution"),tr("Run solution without debug")},
		{ &debugRunProjectAct,    "",                                               "",                                               tr("Debug run &project"),    tr("Debug run project"),    tr("Run project with debug")    },
		{ &noDebugRunProjectAct,  "",                                               "",                                               tr("N&O debug run project"), tr("NO debug run project"), tr("Run project without debug") },
		{ &debugSuspendAct,       ":/images/tango/middle/media-playback-pause.png", ":/images/tango/small/media-playback-pause.png",  tr("All &suspend"),          tr("All suspend"),          tr("Suspend debugging")         },
		{ &debugResumeAct,        ":/images/tango/middle/media-playback-start.png", ":/images/tango/small/media-playback-start.png",  tr("All &resume"),           tr("All resume"),           tr("Resume debugging")          },
		{ &debugStopAct,          ":/images/tango/middle/media-playback-stop.png",  ":/images/tango/small/media-playback-stop.png",   tr("Stop &debugging"),       tr("Stop debugging"),       tr("Abort debugging")           },
		{ &settingAct,            ":/images/tango/middle/preferences-system.png",   ":/images/tango/small/preferences-system.png",    tr("&Setting"),              tr("Setting"),              tr("Application settings")      },
		{ &aboutAct,              ":/images/tango/middle/dialog-information.png",   ":/images/tango/small/dialog-information.png",    tr("&About hspide ..."),     tr("About hspide ..."),     tr("About hspide")              },
	};

	for(size_t i = 0; i < _countof(menuInitTable); i++)
	{
		QAction* action;
		if( menuInitTable[i].middleIcon.isEmpty() ) {
			action = *menuInitTable[i].action = new QAction(menuInitTable[i].text, this);
		} else {
			action = *menuInitTable[i].action = new QAction(QMultiIcon(menuInitTable[i].middleIcon,
			                                                           menuInitTable[i].smallIcon), menuInitTable[i].text, this);
		}
		action->setToolTip(menuInitTable[i].tooltipText);
		action->setStatusTip(menuInitTable[i].statusText);
	}

	connect(newDocumentAct,            SIGNAL(triggered()), this, SLOT(onNewFile()));
	connect(openDocumentAct,           SIGNAL(triggered()), this, SLOT(onOpenFile()));
	connect(saveDocumentAct,           SIGNAL(triggered()), this, SLOT(onSaveFile()));
	connect(saveAsDocumentAct,         SIGNAL(triggered()), this, SLOT(onSaveAsFile()));
	connect(saveAllDocumentAct,        SIGNAL(triggered()), this, SLOT(onSaveAllFile()));
	connect(quitApplicationAct,        SIGNAL(triggered()), this, SLOT(onQuit()));
	connect(QApplication::clipboard(), SIGNAL(dataChanged()),this,SLOT(clipboardDataChanged()));
	connect(gotoLineAct,               SIGNAL(triggered()), this, SLOT(onGoToLine()));
	connect(buildSolutionAct,          SIGNAL(triggered()), this, SLOT(onBuildSolution()));
	connect(buildProjectAct,           SIGNAL(triggered()), this, SLOT(onBuildProject()));
	connect(batchBuildAct,             SIGNAL(triggered()), this, SLOT(onBatchBuild()));
	connect(debugRunSolutionAct,       SIGNAL(triggered()), this, SLOT(onDebugRunSolution()));
	connect(noDebugRunSolutionAct,     SIGNAL(triggered()), this, SLOT(onNoDebugRunSolution()));
	connect(debugRunProjectAct,        SIGNAL(triggered()), this, SLOT(onDebugRunProject()));
	connect(noDebugRunProjectAct,      SIGNAL(triggered()), this, SLOT(onNoDebugRunProject()));
	connect(debugSuspendAct,           SIGNAL(triggered()), this, SLOT(onDebugSuspend()));
	connect(debugResumeAct,            SIGNAL(triggered()), this, SLOT(onDebugResume()));
	connect(debugStopAct,              SIGNAL(triggered()), this, SLOT(onDebugStop()));
	connect(settingAct,                SIGNAL(triggered()), this, SLOT(onOpenSettingDialog()));
	connect(aboutAct,                  SIGNAL(triggered()), this, SLOT(onAboutApp()));
	connect(showProjectDockAct,        SIGNAL(triggered()), this, SLOT(onShowDock()));
	connect(showSymbolDockAct,         SIGNAL(triggered()), this, SLOT(onShowDock()));
	connect(showOutputDockAct,         SIGNAL(triggered()), this, SLOT(onShowDock()));
	connect(showSearchDockAct,         SIGNAL(triggered()), this, SLOT(onShowDock()));
	connect(showMessageDockAct,        SIGNAL(triggered()), this, SLOT(onShowDock()));
	connect(showBreakPointDockAct,     SIGNAL(triggered()), this, SLOT(onShowDock()));
//	connect(showSysInfoDockAct,        SIGNAL(triggered()), this, SLOT(onShowDock()));
//	connect(showVarInfoDockAct,        SIGNAL(triggered()), this, SLOT(onShowDock()));

	showProjectDockAct   ->setData( qVariantFromValue((void*)projectDock) );
	showSymbolDockAct    ->setData( qVariantFromValue((void*)symbolDock) );
	showOutputDockAct    ->setData( qVariantFromValue((void*)outputDock) );
	showSearchDockAct    ->setData( qVariantFromValue((void*)searchDock) );
	showMessageDockAct   ->setData( qVariantFromValue((void*)messageDock) );
	showBreakPointDockAct->setData( qVariantFromValue((void*)breakPointDock) );

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
	debugSuspendAct->setVisible(false);
	debugResumeAct->setEnabled(false);
	debugResumeAct->setVisible(false);
	debugStopAct->setVisible(false);
}

void MainWindow::loadSettings()
{
//	QSettings settings("sharkpp", "hspide");
	QSettings settings("hspide.ini", QSettings::IniFormat);
	settings.setIniCodec("UTF-8"); // 文字コードを指定

	Configuration conf = theConf;
	QVariant tmp;

	// コンパイラパスを取得
	conf.setCompilerPath(settings.value("path/compiler", QDir::currentPath()).toString());
	// HSPディレクトリの取得
	conf.setHspPath(settings.value("path/hsp", QDir::currentPath()).toString());
	// Commonディレクトリの取得
	conf.setHspCommonPath(settings.value("path/hsp-common", conf.hspPath() + "/common/").toString());

	// エディタ関連の設定を取得
	conf.setEditorTabWidth(settings.value("editor/tab-width", conf.editorTabWidth()).toInt());
	conf.setEditorLineNumberVisible(settings.value("editor/line-number-visible", conf.editorLineNumberVisibled()).toBool());
	conf.setEditorLineNumberFontName(settings.value("editor/line-number-font-name", conf.editorLineNumberFontName()).toString());
	conf.setEditorLineNumberFontSize(settings.value("editor/line-number-font-size", conf.editorLineNumberFontSize()).toInt());
	conf.setEditorFontName(settings.value("editor/font-name", conf.editorFontName()).toString());
	conf.setEditorFontSize(settings.value("editor/font-size", conf.editorFontSize()).toInt());

	// 色設定
	QStringList listItems;
	listItems	<< "text"
				<< "line-number"
				<< "function"
				<< "subroutine"
				<< "preprocessor"
				<< "macro"
				<< "label"
				<< "comment"
				<< "string"
				<< "line-feed-char"
				<< "tab-char"
				<< "eof"
				;
	for(int i = 0; i < listItems.size(); i++)
	{
		Configuration::ColorMetricsInfoType metrics = conf.colorMetrics(Configuration::ColorMetricsEnum(i));
		metrics.enable          = settings.value(       QString("editor-color/%1-enable"          ).arg(listItems.at(i)), metrics.enable).toBool();
		metrics.useBoldFont     = settings.value(       QString("editor-color/%1-use-bold-font"   ).arg(listItems.at(i)), metrics.useBoldFont).toBool();
		metrics.backgroundColor = QColor(settings.value(QString("editor-color/%1-foreground-color").arg(listItems.at(i)), metrics.backgroundColor.name()).toString());
		metrics.foregroundColor = QColor(settings.value(QString("editor-color/%1-background-color").arg(listItems.at(i)), metrics.foregroundColor.name()).toString());
		conf.setColorMetrics(Configuration::ColorMetricsEnum(i), metrics);
	}

	// キー割り当て
	QStringList listShortcutItems;
	listShortcutItems
				<< "new"
				<< "open"
				<< "save"
				<< "save-as"
				<< "save-all"
				<< "quit"
				<< "undo"
				<< "redo"
				<< "cut"
				<< "copy"
				<< "paste"
				<< "clear"
				<< "select-all"
				<< "find"
				<< "find-next"
				<< "find-prev"
				<< "replace"
				<< "jump"
				<< "build-solution"
				<< "build-project"
				<< "compile"
				<< "batch-build"
				<< "debug-run-solution"
				<< "no-debug-run-solution"
				<< "debug-run-project"
				<< "no-debug-run-project"
				<< "debug-suspend"
				<< "debug-resume"
				<< "debug-stop"
				<< "config"
				<< "show-project"
				<< "show-symbols"
				<< "show-output"
				<< "show-search"
				<< "show-messages"
				<< "show-breakpoints"
				<< "show-system-variables"
				<< "show-variables"
				;
	for(int i = 0; i < listShortcutItems.size(); i++)
	{
		Configuration::ShortcutInfoType info = conf.shortcut(Configuration::ShortcutEnum(i));
		info.keys = QKeySequence(settings.value(QString("key-assign/%1").arg(listShortcutItems.at(i)), info.keys.toString()).toString());
		conf.setShortcut(Configuration::ShortcutEnum(i), info);
	}

	theConf = conf;

	// ウインドウ位置などを取得
	m_stateDefault      = settings.value("window/state").toByteArray();
	m_stateDebugging    = settings.value("window/state-debugging").toByteArray();
	restoreGeometry(settings.value("window/geometry").toByteArray());
	restoreState(m_stateDefault);
}

void MainWindow::saveSettings()
{
//	QSettings settings("sharkpp", "hspide");
	QSettings settings("hspide.ini", QSettings::IniFormat);
	settings.setIniCodec("UTF-8"); // 文字コードを指定

	QString tmp;

	// コンパイラパスを保存
	settings.setValue("path/compiler", theConf.compilerPath());
	// HSPディレクトリの保存
	settings.setValue("path/hsp", theConf.hspPath());
	// Commonディレクトリの保存
	settings.setValue("path/hsp-common", theConf.hspCommonPath());

	// エディタ関連の設定を保存
	settings.setValue("editor/tab-width", theConf.editorTabWidth());
	settings.setValue("editor/line-number-visible", theConf.editorLineNumberVisibled());
	settings.setValue("editor/line-number-font-name", theConf.editorLineNumberFontName());
	settings.setValue("editor/line-number-font-size", theConf.editorLineNumberFontSize());
	settings.setValue("editor/font-name", theConf.editorFontName());
	settings.setValue("editor/font-size", theConf.editorFontSize());

	// 色設定
	QStringList listItems;
	listItems	<< "text"
				<< "line-number"
				<< "function"
				<< "subroutine"
				<< "preprocessor"
				<< "macro"
				<< "label"
				<< "comment"
				<< "string"
				<< "line-feed-char"
				<< "tab-char"
				<< "eof"
				;
	for(int i = 0; i < listItems.size(); i++)
	{
		const Configuration::ColorMetricsInfoType& metrics = theConf.colorMetrics(Configuration::ColorMetricsEnum(i));
		settings.setValue(QString("editor-color/%1-enable"          ).arg(listItems.at(i)), metrics.enable);
		settings.setValue(QString("editor-color/%1-use-bold-font"   ).arg(listItems.at(i)), metrics.useBoldFont);
		settings.setValue(QString("editor-color/%1-foreground-color").arg(listItems.at(i)), metrics.backgroundColor);
		settings.setValue(QString("editor-color/%1-background-color").arg(listItems.at(i)), metrics.foregroundColor);
	}

	// キー割り当て
	QStringList listShortcutItems;
	listShortcutItems
				<< "new"
				<< "open"
				<< "save"
				<< "save-as"
				<< "save-all"
				<< "quit"
				<< "undo"
				<< "redo"
				<< "cut"
				<< "copy"
				<< "paste"
				<< "clear"
				<< "select-all"
				<< "find"
				<< "find-next"
				<< "find-prev"
				<< "replace"
				<< "jump"
				<< "build-solution"
				<< "build-project"
				<< "compile"
				<< "batch-build"
				<< "debug-run-solution"
				<< "no-debug-run-solution"
				<< "debug-run-project"
				<< "no-debug-run-project"
				<< "debug-suspend"
				<< "debug-resume"
				<< "debug-stop"
				<< "config"
				<< "show-project"
				<< "show-symbols"
				<< "show-output"
				<< "show-search"
				<< "show-messages"
				<< "show-breakpoints"
				<< "show-system-variables"
				<< "show-variables"
				;
	for(int i = 0; i < listShortcutItems.size(); i++)
	{
		const Configuration::ShortcutInfoType& info = theConf.shortcut(Configuration::ShortcutEnum(i));
		settings.setValue(QString("key-assign/%1").arg(listShortcutItems.at(i)), info.keys.toString());
	}

	// ウインドウ位置などを保存
	settings.setValue("window/geometry",        saveGeometry());
	settings.setValue("window/state",           saveState());
	settings.setValue("window/state-debugging", m_stateDebugging);
}

void MainWindow::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	// 終了する前に編集中のファイルを全て保存
	if( !closeSolution() )
	{
			// 終了処理キャンセル
			event->ignore();
			return;
	}

	// デバッグ中の場合はドックを標準状態に戻す
	if( isDebugging() &&
		Qt::NoDockWidgetArea
			!= dockWidgetArea(qobject_cast<QDockWidget*>(sysInfoDock->parentWidget())) )
	{
		endDebugging();
	}
	saveSettings();
	QMainWindow::closeEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if( event->mimeData()->hasUrls() )
	{
		event->acceptProposedAction();
	}
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
	if( event->mimeData()->hasUrls() )
	{
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
			QString filePath = urls.at(i).toLocalFile();
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

void MainWindow::updateConfiguration(const Configuration& info)
{
	theConf.applyShortcut(Configuration::ShortcutNew,					newDocumentAct);
	theConf.applyShortcut(Configuration::ShortcutOpen,					openDocumentAct);
	theConf.applyShortcut(Configuration::ShortcutSave,					saveDocumentAct);
	theConf.applyShortcut(Configuration::ShortcutSaveAs,				saveAsDocumentAct);
	theConf.applyShortcut(Configuration::ShortcutSaveAll,				saveAllDocumentAct);
	theConf.applyShortcut(Configuration::ShortcutQuit,					quitApplicationAct);
	theConf.applyShortcut(Configuration::ShortcutUndo,					editUndoAct);
	theConf.applyShortcut(Configuration::ShortcutRedo,					editRedoAct);
	theConf.applyShortcut(Configuration::ShortcutCut,					editCutAct);
	theConf.applyShortcut(Configuration::ShortcutCopy,					editCopyAct);
	theConf.applyShortcut(Configuration::ShortcutPaste,					editPasteAct);
	theConf.applyShortcut(Configuration::ShortcutClear,					editClearAct);
	theConf.applyShortcut(Configuration::ShortcutSelectAll,				selectAllAct);
	theConf.applyShortcut(Configuration::ShortcutFind,					findTextAct);
	theConf.applyShortcut(Configuration::ShortcutFindNext,				findPrevTextAct);
	theConf.applyShortcut(Configuration::ShortcutFindPrev,				findNextTextAct);
	theConf.applyShortcut(Configuration::ShortcutReplace,				replaceTextAct);
	theConf.applyShortcut(Configuration::ShortcutJump,					gotoLineAct);
	theConf.applyShortcut(Configuration::ShortcutBuildSolution,			buildSolutionAct);
	theConf.applyShortcut(Configuration::ShortcutBuildProject,			buildProjectAct);
	theConf.applyShortcut(Configuration::ShortcutCompileOnly,			compileOnlyAct);
	theConf.applyShortcut(Configuration::ShortcutBatchBuild,			batchBuildAct);
	theConf.applyShortcut(Configuration::ShortcutDebugRunSolution,		debugRunSolutionAct);
	theConf.applyShortcut(Configuration::ShortcutNoDebugRunSolution,	noDebugRunSolutionAct);
	theConf.applyShortcut(Configuration::ShortcutDebugRunProject,		debugRunProjectAct);
	theConf.applyShortcut(Configuration::ShortcutNoDebugRunProject,		noDebugRunProjectAct);
	theConf.applyShortcut(Configuration::ShortcutDebugSuspend,			debugSuspendAct);
	theConf.applyShortcut(Configuration::ShortcutDebugResume,			debugResumeAct);
	theConf.applyShortcut(Configuration::ShortcutDebugStop,				debugStopAct);
	theConf.applyShortcut(Configuration::ShortcutConfig,				settingAct);
	theConf.applyShortcut(Configuration::ShortcutShowProject,			showProjectDockAct);
	theConf.applyShortcut(Configuration::ShortcutShowSymbol,			showSymbolDockAct);
	theConf.applyShortcut(Configuration::ShortcutShowOutput,			showOutputDockAct);
	theConf.applyShortcut(Configuration::ShortcutShowSearch,			showSearchDockAct);
	theConf.applyShortcut(Configuration::ShortcutShowMessage,			showMessageDockAct);
	theConf.applyShortcut(Configuration::ShortcutShowBreakPoint,		showBreakPointDockAct);
//	theConf.applyShortcut(Configuration::ShortcutShowSysInfo,			showSysInfoDockAct);
//	theConf.applyShortcut(Configuration::ShortcutShowVarInfo,			showVarInfoDockAct);
}

void MainWindow::closeTab(CWorkSpaceItem* targetItem)
{
	for(int index = tabWidget->count() - 1; 0 <= index; index--)
	{
		CDocumentPane * document = dynamic_cast<CDocumentPane*>(tabWidget->widget(index));
		CWorkSpaceItem* item = document->assignItem();
		if( !targetItem || targetItem == item )
		{
			tabWidget->removeTab(index);
			// 全て削除の場合以外は対象のタブが見つかったら終了
			if( targetItem ) {
				break;
			}
		}
	}
}

bool MainWindow::closeSolution()
{
	CSaveSolutionDialog dlg(this);

	CWorkSpaceItem* solutionItem = projectDock->currentSolution();
	
	if( dlg.setSolution(solutionItem) )
	{
		switch( dlg.exec() ) 
		{
		default:
		case QDialogButtonBox::No:
			// 保存しない
			break;
		case QDialogButtonBox::Yes: {
			QList<CSaveSolutionDialog::SavingItemInfo> list = dlg.list();
			foreach(CSaveSolutionDialog::SavingItemInfo listItem, list) {
				if( listItem.first->save(listItem.second) )
				{
				}
			}
			break; }
		case QDialogButtonBox::Cancel: 
			// キャンセル
			return false;
		}
	}

	return true;
}

CWorkSpaceItem* MainWindow::currentItem()
{
	CDocumentPane * document = static_cast<CDocumentPane*>(tabWidget->currentWidget());
	CWorkSpaceItem* item = NULL;

	if( document )
	{
		// 現在選択しているタブに関連付けられているファイルを取得
		item = document->assignItem();
	}
	else
	{
		// なければ、プロジェクトツリーから選択しているファイルを取得
		item = projectDock->currentItem();
	}

	return item;
}

void MainWindow::onAboutApp()
{
//	CAboutDialog dlg(this);
	QDialog dlg(this);
	Ui::AboutDialog ui;
	ui.setupUi(&dlg);
	dlg.exec();
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
	tabWidget->addTab(document, document->title());
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
			// ソリューションファイルの場合はすでに開いているソリューションを閉じて開く
			CWorkSpaceItem * solutionItem = projectDock->currentSolution();
			// ソリューションを閉じる
			if( closeSolution() )
			{
				// 今のタブを全て閉じて
				closeTab();
				// プロジェクトを開く
				if( solutionItem->load(fileName) )
				{
					QList<CWorkSpaceItem*> r(projectDock->projects());
					for(int i = 0, num = r.count(); i < num; i++) {
						onOpenFile(r[i]);
					}
				}
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
	if( !item )
	{
		return;
	}

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
	tabWidget->addTab(document, document->title());
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
			tabWidget->setTabText(tabWidget->currentIndex(), document->title());
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

void MainWindow::onGoToLine(const QUuid & uuid, int lineNo)
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
			document->jump(lineNo);
		}

		activateWindow();
	}
}

void MainWindow::onStopAtBreakPoint(const QUuid & uuid, int lineNo)
{
	onGoToLine(uuid, lineNo);

	// メニューを変更
	debugResumeAct->setEnabled(true);
	debugSuspendAct->setEnabled(false);
	varInfoDock->setEnable(true);
	sysInfoDock->setEnable(true);
}

void MainWindow::onUpdateDebugInfo(const QVector<QPair<QString,QString> > & info)
{
	typedef QPair<QString,QString> string_pair; // 直接だとマクロの展開エラーになるので...
	foreach(const string_pair& val, info) {
		sysInfoDock->update(val.first, val.second);
	}
}

void MainWindow::onUpdateVarInfo(const QVector<VARIABLE_INFO_TYPE> & info)
{
	foreach(const VARIABLE_INFO_TYPE& var, info) {
		varInfoDock->update(var);
	}
}

void MainWindow::onReqVarInfo(const QString& varName, int* info)
{
	CDebugger* debugger = NULL;
	// とりあえず...
	foreach(CDebugger* debugger_, m_debuggers) {
		debugger = debugger_;
	}
	if( debugger )
	{
		debugger->reqVariableInfo(varName, info);
	}
}

void MainWindow::onBuildProject()
{
	CWorkSpaceItem* item = currentItem();

	if( !item )
	{
		return;
	}

	// もし、プロジェクトに属していたら取得
	CWorkSpaceItem* currentProject
		= item->ancestor(CWorkSpaceItem::Project);

	if( currentProject )
	{
		// プロジェクトが取得できたらビルド＆実行
		m_compiler->build(currentProject, true);
	}
}

void MainWindow::onBuildSolution()
{
	CWorkSpaceItem* item = currentItem();

	if( !item )
	{
		return;
	}

	// もし、ソリューションに属していたら取得
	CWorkSpaceItem* currentSolution
		= item->ancestor(CWorkSpaceItem::Solution);

	if( currentSolution )
	{
		// ソリューションが取得できたらソリューション構成を取得しビルド＆実行
		m_compiler->build(currentSolution, true);
	}
}

void MainWindow::onBatchBuild()
{
}

void MainWindow::onDebugRunSolution()
{
	CWorkSpaceItem* item = currentItem();

	if( !item )
	{
		return;
	}

	// もし、ソリューションに属していたら取得
	CWorkSpaceItem* currentSolution
		= item->ancestor(CWorkSpaceItem::Solution);

	if( currentSolution )
	{
		// ソリューションが取得できたらソリューション構成を取得しビルド＆実行
		m_compiler->run(currentSolution, true);
	}
}

void MainWindow::onNoDebugRunSolution()
{
	CWorkSpaceItem* item = currentItem();

	if( !item )
	{
		return;
	}

	// もし、ソリューションに属していたら取得
	CWorkSpaceItem* currentSolution
		= item->ancestor(CWorkSpaceItem::Solution);

	if( currentSolution )
	{
		// ソリューションが取得できたらソリューション構成を取得しビルド＆実行
		m_compiler->run(currentSolution, false);
	}
}

void MainWindow::onDebugRunProject()
{
	CWorkSpaceItem* item = currentItem();

	if( !item )
	{
		return;
	}

	// もし、プロジェクトに属していたら取得
	CWorkSpaceItem* currentProject
		= item->ancestor(CWorkSpaceItem::Project);

	if( currentProject )
	{
		// プロジェクトが取得できたらビルド＆実行
		m_compiler->run(currentProject, true);
	}
}

void MainWindow::onNoDebugRunProject()
{
	CWorkSpaceItem* item = currentItem();

	if( !item )
	{
		return;
	}

	// もし、プロジェクトに属していたら取得
	CWorkSpaceItem* currentProject
		= item->ancestor(CWorkSpaceItem::Project);

	if( currentProject )
	{
		// プロジェクトが取得できたらビルド＆実行
		m_compiler->run(currentProject, false);
	}
}

void MainWindow::onDebugSuspend()
{
	// 全てのデバッグを中断
	foreach(CDebugger* debugger, m_debuggers) {
		debugger->suspendDebugging();
	}

	// メニューを変更
	debugResumeAct->setEnabled(true);
	debugSuspendAct->setEnabled(false);
	varInfoDock->setEnable(true);
	sysInfoDock->setEnable(true);
}

void MainWindow::onDebugResume()
{
	// 全てのデバッグを再開
	foreach(CDebugger* debugger, m_debuggers) {
		debugger->resumeDebugging();
	}

	// メニューを変更
	debugResumeAct->setEnabled(false);
	debugSuspendAct->setEnabled(true);
	varInfoDock->setEnable(false);
	sysInfoDock->setEnable(false);
}

void MainWindow::onDebugStop()
{
	// 全てのデバッグを停止
	foreach(CDebugger* debugger, m_debuggers) {
		debugger->stopDebugging();
	}
}

void MainWindow::onOpenSettingDialog()
{
	CConfigDialog dlg(this);

	if( QDialog::Accepted == dlg.exec() )
	{
		theConf = dlg.configuration();
	}
}

void MainWindow::onTabListShow()
{
    m_tabListMenu->clear();

	for(int index = 0; index < tabWidget->count(); index++)
	{
		CDocumentPane* document
			= dynamic_cast<CDocumentPane*>(tabWidget->widget(index));
		if( document &&
			document->assignItem())
		{
			CWorkSpaceItem* item = document->assignItem();
			QAction* action = new QAction(item->text(), this);
			// クリック時にタブをアクティブに出来るように
			// 情報を保持
			action->setData(item->uuid().toString());
			// アクティブなタブを強調
			if( index == tabWidget->currentIndex() )
			{
				QFont font = action->font();
				font.setBold(true);
				action->setFont(font);
			}
			// クリック時にタブをアクティブに出来るように
			connect(action, SIGNAL(triggered()), this, SLOT(onTabListClicked()));
			// メニューに追加
			m_tabListMenu->addAction(action);
		}
	}
}

void MainWindow::onTabListClicked()
{
	QAction* action = qobject_cast<QAction*>(sender());
	QUuid uuid(action->data().toString());

	for(int index = 0; index < tabWidget->count(); index++)
	{
		CDocumentPane* document
			= dynamic_cast<CDocumentPane*>(tabWidget->widget(index));
		if( document &&
			document->assignItem() &&
			uuid == document->assignItem()->uuid() )
		{
			tabWidget->setCurrentIndex(index);
			break;
		}
	}
}

void MainWindow::onTabList()
{
	// メニューの表示位置を取得
	QWidget* tabButton = tabWidget->cornerWidget(Qt::TopRightCorner);
	QPoint pt;
	for(QWidget* p = tabButton; p; p = p->parentWidget()) {
		pt.setX(pt.x() + p->geometry().x());
		pt.setY(pt.y() + p->geometry().y());
	}
	pt.setY(pt.y() + tabButton->height());
	// メニュー表示
	m_tabListMenu->popup(pt);
}

// タブを閉じる
void MainWindow::onTabClose()
{
	// 何はともあれ保存
	onSaveFile();

	CDocumentPane * document = static_cast<CDocumentPane*>(tabWidget->currentWidget());

	CWorkSpaceItem * solutionItem = projectDock->currentSolution();
	CWorkSpaceItem * currentItem  = document->assignItem();

	// タブを閉じる
	closeTab(currentItem);

	// 関連付けを解除
	currentItem->setAssignDocument(NULL);

	// ソリューションが空もしくはプロジェクトに名前を
	// つけていない場合はツリーから削除
	if( (solutionItem && solutionItem->isUntitled()) ||
		currentItem->isUntitled() )
	{
		m_workSpace->remove(currentItem);
	}
}

void MainWindow::buildStart(int buildOrder, const QString & filePath)
{
	// ビルド処理開始

	outputDock->select(COutputDock::BuildOutput);
	outputDock->outputCrLf(COutputDock::BuildOutput,
	                       QString("%1>").arg(buildOrder + 1) + tr("Build start"));

	CWorkSpaceItem* targetProjectItem
		= projectDock->currentSolution()->search(filePath);

	// ビルドを開始したプロジェクトに関連するファイルを全て保存
	if( targetProjectItem )
	{
		// ファイルを保存
		for(int index = 0, num = tabWidget->count(); index < num; index++)
		{
			CDocumentPane * document = dynamic_cast<CDocumentPane*>(tabWidget->widget(index));
			CWorkSpaceItem* item = document->assignItem();
			if( targetProjectItem ==
					item->ancestor(CWorkSpaceItem::Project) )
			{
				if( !document->isUntitled() )
				{
					document->save();
				}
			}
		}
	}

	// プログラスバーをMarqueeスタイルに
	taskProgress->setRange(0, 0);
	taskProgress->setVisible(true);

	// 出力をクリア
	outputDock->clear(COutputDock::BuildOutput);
	messageDock->clear();
}

void MainWindow::buildFinished(int buildOrder, bool successed)
{
	// ビルド処理完了

	taskProgress->setVisible(false);

	QString outputContents = QString("%1>").arg(buildOrder + 1);

	if( !successed )
	{
		outputContents += tr("Build failed");
	}
	else
	{
		outputContents += tr("Build complete");
	}

	outputDock->outputCrLf(COutputDock::BuildOutput, outputContents);
}

// ビルド中の出力を取得
void MainWindow::buildOutput(int buildOrder, const QString & output)
{
	CWorkSpaceItem* solutionItem = projectDock->currentSolution();

	QString tmp = QString(output).replace("\r\n", "\n");

	QString tmp2 = tmp;

	// uuidをファイル名に変換

	QRegExp reFileName("\\?(\\{[0-9a-fA-F-]+\\})");

	// uuidをファイル名に置換
	if( 0 <= reFileName.indexIn(tmp2) ) {
		QString uuid = reFileName.cap(1);
		CWorkSpaceItem* item = solutionItem ? solutionItem->search(QUuid(uuid)) : NULL;
		if( item ) {
			tmp2.replace(reFileName, item->text());
		}
	}

	// ビルド順を追加
	QString outputContents;
	for(QStringListIterator ite(tmp2.split("\n"));
		ite.hasNext(); )
	{
		QString line = ite.next();
		line.replace("\r", "");
		outputContents += QString("%1>%2\n").arg(buildOrder + 1).arg(line);
	}

	// 出力ドックに内容を送信
	outputDock->output(COutputDock::BuildOutput, outputContents);

	// メッセージドックに出力する内容を生成

	bool raiseDock = false;

	QRegExp compileError("^((.+)\\(([0-9]+)\\) : (.+))$");
	QRegExp preProcessorError("^#Error:(.+) in line ([0-9]+) \\[(.*)\\]$");
//	QRegExp warning("^(.+):(.+)( 行|at )([0-9]+)\\(.\\[.+\\])$");
	QRegExp excludeInfo("^#(HSP script preprocessor|HSP code generator)");
	QRegExp trimBuildOrder("^[0-9]+>(.+)");

	for(QStringListIterator ite(tmp.split("\n"));
		ite.hasNext(); )
	{
		QString line = ite.next();
		line.replace("\r", "");

		if( 0 <= trimBuildOrder.indexIn(line) ) {
			line = trimBuildOrder.cap(1);
		}

		bool accepted = false;

		for(int i = 0; i < 3 && !accepted; i++)
		{
			CMessageDock::CategoryType type;
			QString fileName, lineNo, desc;
			QUuid uuid;

			switch(i)
			{
			case 0: // コンパイルエラー
				if( compileError.indexIn(line) < 0 ) {
					continue;
				}
				fileName = compileError.cap(2);
				lineNo   = compileError.cap(3);
				desc     = compileError.cap(4);
				type     = CMessageDock::ErrorCategory;
				raiseDock= true;
				break;
			case 1: // プリプロセスエラー
				if( preProcessorError.indexIn(line) < 0 ) {
					continue;
				}
				fileName = preProcessorError.cap(3);
				lineNo   = preProcessorError.cap(2);
				desc     = preProcessorError.cap(1);
				type     = CMessageDock::ErrorCategory;
				raiseDock= true;
				break;
//			case 2: // 警告
//				if( warning.indexIn(line) < 0 ) {
//					continue;
//				}
//				fileName = "";
//				lineNo   = warning.cap(4);
//				desc     = warning.cap(2) + warning.cap(5);
//				type     = CMessageDock::WarningCategory;
//				break;
			case 2:
				if( 0 <= excludeInfo.indexIn(line) ||
#ifdef _DEBUG
					"!!! " == line.left(4) ||
#endif
					"#" != line.left(1) ) {
					continue;
				}
				fileName = "";
				lineNo   = "-1";
				desc     = line;
				type     = CMessageDock::InfomationCategory;
				break;
			}
			// UUIDをファイル名に変換 or ファイル名をUUIDに変換
			if( 0 <= reFileName.indexIn(fileName) ) {
qDebug()<<">>"<<reFileName.cap(1);
				uuid = theFile.uuidFromFilename(reFileName.cap(1));
			} else {
				if( CWorkSpaceItem* item
						= solutionItem ? solutionItem->search(fileName)
						               : NULL )
				{
					uuid = item->uuid();
				}
			}
			// メッセージを追加
			messageDock->addMessage(uuid, lineNo.toInt(), type, desc);
			accepted  = true;
		}
	}

	// ドックが隠れていたら表に表示
	if( raiseDock )
	{
		QDockWidget* dock = qobject_cast<QDockWidget*>(messageDock->parentWidget());
		dock->raise();
	}
}

void MainWindow::onUpdatedSymbols()
{
	CCompiler* compiler = qobject_cast<CCompiler*>(sender());

	// 開いているタブ全てにキーワードを再登録
	for(int index = 0; index < tabWidget->count(); index++)
	{
		CDocumentPane* document
			= dynamic_cast<CDocumentPane*>(tabWidget->widget(index));
		if( document )
		{
			document->setSymbols(compiler->symbols());
		}
	}
}

void MainWindow::attachedDebugger(CDebugger* debugger)
{
	connect(debugger, SIGNAL(stopAtBreakPoint(const QUuid&,int)),                       this, SLOT(onStopAtBreakPoint(const QUuid &,int)));
	connect(debugger, SIGNAL(putLog(const QString&)),                                   this, SLOT(onPutDebugLog(const QString&)));
	connect(debugger, SIGNAL(updateDebugInfo(const QVector<QPair<QString,QString> >&)), this, SLOT(onUpdateDebugInfo(const QVector<QPair<QString,QString> > &)));
	connect(debugger, SIGNAL(updateVarInfo(const QVector<VARIABLE_INFO_TYPE>&)),        this, SLOT(onUpdateVarInfo(const QVector<VARIABLE_INFO_TYPE> &)));
	connect(debugger, SIGNAL(destroyed()),                                              this, SLOT(dettachedDebugger()));

	debugRunSolutionAct  ->setVisible( !m_debuggers.empty() );
	noDebugRunSolutionAct->setVisible( !m_debuggers.empty() );
	debugRunProjectAct   ->setVisible( !m_debuggers.empty() );
	noDebugRunProjectAct ->setVisible( !m_debuggers.empty() );
	debugSuspendAct      ->setVisible(  m_debuggers.empty() );
	debugResumeAct       ->setVisible(  m_debuggers.empty() );
	debugStopAct         ->setVisible(  m_debuggers.empty() );

	if( m_debuggers.empty() ) {
		// デバッグ開始
		beginDebugging();
	}

	m_debuggers.insert(debugger);
}

void MainWindow::dettachedDebugger()
{
	CDebugger* debugger = reinterpret_cast<CDebugger*>(sender());

	m_debuggers.remove(debugger);

	debugRunSolutionAct  ->setVisible(  m_debuggers.empty() );
	noDebugRunSolutionAct->setVisible(  m_debuggers.empty() );
	debugRunProjectAct   ->setVisible(  m_debuggers.empty() );
	noDebugRunProjectAct ->setVisible(  m_debuggers.empty() );
	debugSuspendAct      ->setVisible( !m_debuggers.empty() );
	debugResumeAct       ->setVisible( !m_debuggers.empty() );
	debugStopAct         ->setVisible( !m_debuggers.empty() );

	if( m_debuggers.empty() ) {
		// デバッグ終了
		endDebugging();
	}
}

bool MainWindow::isDebugging() const
{
	return !m_debuggers.empty();
}

void MainWindow::beginDebugging()
{
	outputDock->select(COutputDock::DebugOutput);
	outputDock->clear(COutputDock::DebugOutput);

	// 最初のデバッガの場合のみクリア
	sysInfoDock->clear();
	sysInfoDock->setEnable(false);
	varInfoDock->clear();
	varInfoDock->setEnable(false);
	// メニューを変更
	debugResumeAct->setEnabled(false);
	debugSuspendAct->setEnabled(true);

	// 現在のレイアウトを保存
	m_stateDefault = saveState();
	// デバッグ時にしか存在しないドックを追加
	QDockWidget* sysInfoDockWidget    = qobject_cast<QDockWidget*>(sysInfoDock->parentWidget());
	QDockWidget* varInfoDockWidget    = qobject_cast<QDockWidget*>(varInfoDock->parentWidget());
	QDockWidget* outputDockWidget     = qobject_cast<QDockWidget*>(outputDock->parentWidget());
	addDockWidget(Qt::BottomDockWidgetArea, sysInfoDockWidget);
	addDockWidget(Qt::BottomDockWidgetArea, varInfoDockWidget);
	tabifyDockWidget(outputDockWidget, sysInfoDockWidget);
	tabifyDockWidget(outputDockWidget, varInfoDockWidget);
	sysInfoDockWidget->setVisible(true);
	varInfoDockWidget->setVisible(true);
	// デバッグ用のレイアウトに変更
	restoreState(m_stateDebugging);
}

void MainWindow::endDebugging()
{
	sysInfoDock->setEnable(false);
	varInfoDock->setEnable(false);

	// 現在のレイアウトを保存
	m_stateDebugging = saveState();
	// デバッグ時にしか存在しないドックを取り除く
	QDockWidget* sysInfoDockWidget = qobject_cast<QDockWidget*>(sysInfoDock->parentWidget());
	QDockWidget* varInfoDockWidget = qobject_cast<QDockWidget*>(varInfoDock->parentWidget());
	removeDockWidget(sysInfoDockWidget);
	removeDockWidget(varInfoDockWidget);
	sysInfoDockWidget->setVisible(false);
	varInfoDockWidget->setVisible(false);
	// 通常のレイアウトに変更
	restoreState(m_stateDefault);
}

void MainWindow::onPutDebugLog(const QString& text)
{
	// 出力ドックに内容を送信
	outputDock->outputCrLf(COutputDock::DebugOutput, text);
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

// ブレークポイントが更新された
void MainWindow::onUpdateBreakpoint()
{
	// 全てのデバッグを中断
	foreach(CDebugger* debugger, m_debuggers) {
		debugger->updateBreakpoint();
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
		disconnect(lastDocument,                       SIGNAL(updateBreakpoint()));
		disconnect(lastDocument->editor()->document(), SIGNAL(undoAvailable(bool)));
		disconnect(lastDocument->editor()->document(), SIGNAL(redoAvailable(bool)));
		disconnect(lastDocument->editor(),             SIGNAL(copyAvailable(bool)));
		disconnect(editUndoAct,                        SIGNAL(triggered()));
		disconnect(editRedoAct,                        SIGNAL(triggered()));
		disconnect(editCopyAct,                        SIGNAL(triggered()));
		disconnect(editPasteAct,                       SIGNAL(triggered()));
		disconnect(editCutAct,                         SIGNAL(triggered()));
		disconnect(editClearAct,                       SIGNAL(triggered()));
		disconnect(selectAllAct,                       SIGNAL(triggered()));

		symbolDock->setAssignDocument(NULL);
	}

	if( document )
	{
		// シグナルと関連付け
		connect(document,                       SIGNAL(modificationChanged(bool)), this,               SLOT(onDocumentChanged(bool)));
		connect(document,                       SIGNAL(modificationChanged(bool)), saveDocumentAct,    SLOT(setEnabled(bool)));
		connect(document,                       SIGNAL(updateBreakpoint()),        this,               SLOT(onUpdateBreakpoint()));
		connect(document->editor()->document(), SIGNAL(undoAvailable(bool)),       editUndoAct,        SLOT(setEnabled(bool)));
		connect(document->editor()->document(), SIGNAL(redoAvailable(bool)),       editRedoAct,        SLOT(setEnabled(bool)));
		connect(document->editor(),             SIGNAL(copyAvailable(bool)),       editCutAct,         SLOT(setEnabled(bool)));
		connect(document->editor(),             SIGNAL(copyAvailable(bool)),       editCopyAct,        SLOT(setEnabled(bool)));
		connect(document->editor(),             SIGNAL(copyAvailable(bool)),       editClearAct,       SLOT(setEnabled(bool)));
		connect(editUndoAct,                    SIGNAL(triggered()),               document->editor(), SLOT(undo()));
		connect(editRedoAct,                    SIGNAL(triggered()),               document->editor(), SLOT(redo()));
		connect(editCopyAct,                    SIGNAL(triggered()),               document->editor(), SLOT(copy()));
		connect(editPasteAct,                   SIGNAL(triggered()),               document->editor(), SLOT(paste()));
		connect(editCutAct,                     SIGNAL(triggered()),               document->editor(), SLOT(cut()));
		connect(editClearAct,                   SIGNAL(triggered()),               document->editor(), SLOT(del()));
		connect(selectAllAct,                   SIGNAL(triggered()),               document->editor(), SLOT(selectAll()));

		document->setFocus();

		symbolDock->setAssignDocument(document);

		projectDock->selectItem(document->assignItem());
	}

	saveDocumentAct->   setEnabled( document ? document->isModified() : false );
	saveAsDocumentAct-> setEnabled( NULL != document );
	saveAllDocumentAct->setEnabled( NULL != document );

	m_lastActivatedDocument = document;

	onDocumentChanged();
	clipboardDataChanged();
}

void MainWindow::clipboardDataChanged()
{
	CDocumentPane* document
		= dynamic_cast<CDocumentPane*>(tabWidget->currentWidget());
	const QMimeData *md = QApplication::clipboard()->mimeData();

	editPasteAct->setEnabled(document && md && md->hasText());
}

void MainWindow::onDocumentChanged(bool)
{
	CDocumentPane* document
		= dynamic_cast<CDocumentPane*>(sender());

	int index = tabWidget->indexOf(document);
	if( 0 <= index )
	{
		tabWidget->setTabText(tabWidget->currentIndex(), document->title());
	}
}

void MainWindow::onSymbolDockVisibilityChanged(bool visible)
{
	if( visible )
	{
		symbolDock->update();
	}
}

void MainWindow::onShowDock()
{
	QAction* action = qobject_cast<QAction*>(sender());
	QWidget* dock   = (QWidget*)action->data().value<void*>();
	QDockWidget* dockWidget = qobject_cast<QDockWidget*>(dock->parentWidget());
	dockWidget->setVisible(true);
	dockWidget->raise();
}

