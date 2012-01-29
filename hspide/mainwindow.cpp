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

	// �����������̒ʒm��o�^
	connect(m_compiler, SIGNAL(buildStart(const QString &)),  this, SLOT(buildStart(const QString &)));
	connect(m_compiler, SIGNAL(buildFinished(bool)),          this, SLOT(buildFinished(bool)));
	connect(m_compiler, SIGNAL(buildOutput(const QString &)), this, SLOT(buildOutput(const QString &)));
	connect(m_compiler, SIGNAL(attachedDebugger(CDebugger*)), this, SLOT(attachedDebugger(CDebugger*)));

	tabWidget = new QTabWidget(this);
	setCentralWidget(tabWidget);

	// �^�u�̈ꗗ�ƕ���{�^��
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

	connect(tabWidget,   SIGNAL(currentChanged(int)),                        this, SLOT(currentTabChanged(int)));
	connect(projectDock, SIGNAL(oepnItem(CWorkSpaceItem*)),                  this, SLOT(onOpenFile(CWorkSpaceItem *)));
	connect(tabListAct,  SIGNAL(triggered()),                                this, SLOT(onTabList()));
	connect(tabCloseAct, SIGNAL(triggered()),                                this, SLOT(onTabClose()));
	connect(messageDock, SIGNAL(gotoLine(const QUuid &, int)),               this, SLOT(onGoToLine(const QUuid &, int)));
	connect(symbolDock,  SIGNAL(gotoLine(const QUuid &, int)),               this, SLOT(onGoToLine(const QUuid &, int)));
	connect(varInfoDock, SIGNAL(requestVariableInfo(const QString& , int*)), this, SLOT(onReqVarInfo(const QString& , int*)));
	connect(qobject_cast<QDockWidget*>(symbolDock->parentWidget()),
	                     SIGNAL(visibilityChanged(bool)),                    this, SLOT(onSymbolDockVisibilityChanged(bool)));

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
	projectDockWidget->setObjectName("Project"); // saveState()�Ōx�����g���[�X�ŏo�邽��
	addDockWidget(Qt::LeftDockWidgetArea, projectDockWidget);

	QDockWidget* symbolDockWidget = new QDockWidget(tr("Symbol list"), this);
	symbolDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	symbolDockWidget->setWidget(symbolDock = new CSymbolDock(symbolDockWidget));
	symbolDockWidget->setObjectName("Symbol list"); // saveState()�Ōx�����g���[�X�ŏo�邽��
	addDockWidget(Qt::LeftDockWidgetArea, symbolDockWidget);

	QDockWidget* outputDockWidget = new QDockWidget(tr("Output"), this);
	outputDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	outputDockWidget->setWidget(outputDock = new COutputDock(outputDockWidget));
	outputDockWidget->setObjectName("Output"); // saveState()�Ōx�����g���[�X�ŏo�邽��
	addDockWidget(Qt::BottomDockWidgetArea, outputDockWidget);

	QDockWidget* searchDockWidget = new QDockWidget(tr("Search result"), this);
	searchDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	searchDockWidget->setWidget(searchDock = new CSearchDock(searchDockWidget));
	searchDockWidget->setObjectName("Search result"); // saveState()�Ōx�����g���[�X�ŏo�邽��
	addDockWidget(Qt::BottomDockWidgetArea, searchDockWidget);

	QDockWidget* messageDockWidget = new QDockWidget(tr("Messages"), this);
	messageDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	messageDockWidget->setWidget(messageDock = new CMessageDock(messageDockWidget));
	messageDockWidget->setObjectName("Messages"); // saveState()�Ōx�����g���[�X�ŏo�邽��
	addDockWidget(Qt::BottomDockWidgetArea, messageDockWidget);

	// �h�b�N�^�u�Ɍ���
	tabifyDockWidget(projectDockWidget, symbolDockWidget);
	tabifyDockWidget(outputDockWidget, searchDockWidget);
	tabifyDockWidget(outputDockWidget, messageDockWidget);

//	projectDockWidget->setFocus();
	projectDockWidget->raise();

//	outputDockWidget->setFocus();
	outputDockWidget->raise();

 // �f�o�b�O���̂ݗL���ȃh�b�N

	QDockWidget* sysInfoDockWidget = new QDockWidget(tr("SystemInfo"), this);
	sysInfoDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	sysInfoDockWidget->setWidget(sysInfoDock = new CSystemInfoDock(sysInfoDockWidget));
	sysInfoDockWidget->setObjectName("SystemInfo"); // saveState()�Ōx�����g���[�X�ŏo�邽��
	sysInfoDockWidget->setVisible(false);

	QDockWidget* varInfoDockWidget = new QDockWidget(tr("VariableInfo"), this);
	varInfoDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	varInfoDockWidget->setWidget(varInfoDock = new CVariableInfoDock(varInfoDockWidget));
	varInfoDockWidget->setObjectName("VariableInfo"); // saveState()�Ōx�����g���[�X�ŏo�邽��
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
}

void MainWindow::setupToolBars()
{
	QToolBar * generalToolbar = addToolBar(tr("General"));
		generalToolbar->setObjectName("General"); // saveState()�Ōx�����g���[�X�ŏo�邽��
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
		debugMenu->addAction(debugResumeAct);
		debugMenu->addAction(debugSuspendAct);
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
	connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));

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

	debugSuspendAct = new QAction(QMultiIcon(":/images/tango/middle/media-playback-pause.png",
	                                         ":/images/tango/small/media-playback-pause.png"), tr("All &suspend"), this);
//	debugSuspendAct->setShortcuts(QKeySequence::Replace);
	debugSuspendAct->setStatusTip(tr("Run program without debug"));
	connect(debugSuspendAct, SIGNAL(triggered()), this, SLOT(onDebugSuspend()));

	debugResumeAct = new QAction(QMultiIcon(":/images/tango/middle/media-playback-start.png",
	                                        ":/images/tango/small/media-playback-start.png"), tr("All &resume"), this);
//	debugResumeAct->setShortcuts(QKeySequence::Replace);
	debugResumeAct->setStatusTip(tr("Run program without debug"));
	connect(debugResumeAct, SIGNAL(triggered()), this, SLOT(onDebugResume()));

	debugStopAct = new QAction(QMultiIcon(":/images/tango/middle/media-playback-stop.png",
	                                      ":/images/tango/small/media-playback-stop.png"), tr("Stop &debugging"), this);
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
	debugSuspendAct->setVisible(false);
	debugResumeAct->setEnabled(false);
	debugResumeAct->setVisible(false);
	debugStopAct->setVisible(false);
}

void MainWindow::loadSettings()
{
//	QSettings settings("sharkpp", "hspide");
	QSettings settings("hspide.ini", QSettings::IniFormat);
	settings.setIniCodec("UTF-8"); // �����R�[�h���w��

	QVariant tmp;

	// �R���p�C���p�X���擾
	m_compiler->setCompilerPath(settings.value("path/compiler", QDir::currentPath()).toString());
	// HSP�f�B���N�g���̎擾
	m_compiler->setHspPath(settings.value("path/hsp", QDir::currentPath()).toString());
	// Common�f�B���N�g���̎擾
	m_compiler->setHspCommonPath(settings.value("path/hsp-common", m_compiler->hspPath() + "/common/").toString());

	// �E�C���h�E�ʒu�Ȃǂ��擾
	m_stateDefault      = settings.value("window/state").toByteArray();
	m_stateDebugging    = settings.value("window/state-debugging").toByteArray();
	restoreGeometry(settings.value("window/geometry").toByteArray());
	restoreState(m_stateDefault);
}

void MainWindow::saveSettings()
{
//	QSettings settings("sharkpp", "hspide");
	QSettings settings("hspide.ini", QSettings::IniFormat);
	settings.setIniCodec("UTF-8"); // �����R�[�h���w��

	QString tmp;

	// �R���p�C���p�X��ۑ�
	settings.setValue("path/compiler", m_compiler->compilerPath());
	// HSP�f�B���N�g���̕ۑ�
	settings.setValue("path/hsp", m_compiler->hspPath());
	// Common�f�B���N�g���̕ۑ�
	settings.setValue("path/hsp-common", m_compiler->hspCommonPath());

	// �E�C���h�E�ʒu�Ȃǂ�ۑ�
	settings.setValue("window/geometry", saveGeometry());
	settings.setValue("window/state",           saveState());
	settings.setValue("window/state-debugging", m_stateDebugging);
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
	// �f�o�b�O���̏ꍇ�̓h�b�N��W����Ԃɖ߂�
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
		// �w�萔�ȏ�̏ꍇ�A�x�����b�Z�[�W��\��
		// ����͎�芸�����̎���
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
				// �J���Ă���r���ɐi�s�󋵂��o������
				// ���łɒ��~�{�^����������Ƃ�������
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
		// �t�@�C�����������t�@�C���̏ꍇ�͂��ƂŃt�@�C���ۑ��_�C�A���O���o���œo�^
		break;
	default:
		return;
	}
	// �\�����[�V�����Ƀv���W�F�N�g��ǉ����A
	// �^�u�Œǉ������t�@�C�����J��
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

	// �t�@�C�������w�肳��Ă��Ȃ�������J��
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
	// ���łɊJ���Ă�����^�u��I�����Ȃ���
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

	// �_�u���N���b�N�ŊJ����̂̓t�@�C���̂�
	if( CWorkSpaceItem::FileNode != item->nodeType() )
	{
		return;
	}

	// �V���ɊJ��
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
			// �t�@�C�������X�V
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
		// �s���ړ�
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
		// �t�@�C�����J��
		onOpenFile(targetProjectItem);

		// �s���ړ�
		CDocumentPane* document
			= dynamic_cast<CDocumentPane*>(tabWidget->currentWidget());
		if( document )
		{
			document->jump(lineNo);
		}

		activateWindow();
	}
}

void MainWindow::onUpdateDebugInfo(const QVector<QPair<QString,QString> > & info)
{
	typedef QPair<QString,QString> string_pair; // ���ڂ��ƃ}�N���̓W�J�G���[�ɂȂ�̂�...
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
	// �Ƃ肠����...
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
		// ���ݑI�����Ă���^�u�Ɋ֘A�t�����Ă���t�@�C�����擾
		currentItem = document->assignItem();
	}
	else
	{
		// �Ȃ���΁A�v���W�F�N�g�c���[����I�����Ă���t�@�C�����擾
		currentItem = projectDock->currentItem();
	}

	if( !currentItem )
	{
		return;
	}

	// �����A�v���W�F�N�g�ɑ����Ă�����擾
	CWorkSpaceItem* currentProject
		= currentItem->ancestor(CWorkSpaceItem::Project);

	// �����A�\�����[�V�����ɑ����Ă�����擾
	CWorkSpaceItem* currentSolution
		= currentItem->ancestor(CWorkSpaceItem::Solution);

	if( currentSolution &&
		0 )
	{
		// �\�����[�V�������擾�ł�����\�����[�V�����\�����擾���r���h�����s
		
	}
	else if( currentProject )
	{
		// �v���W�F�N�g���擾�ł�����r���h�����s
		m_compiler->run(currentProject, true);
	}
}

void MainWindow::onNoDebugRun()
{
	CDocumentPane * document = static_cast<CDocumentPane*>(tabWidget->currentWidget());
	CWorkSpaceItem* currentItem = NULL;

	if( document )
	{
		// ���ݑI�����Ă���^�u�Ɋ֘A�t�����Ă���t�@�C�����擾
		currentItem = document->assignItem();
	}
	else
	{
		// �Ȃ���΁A�v���W�F�N�g�c���[����I�����Ă���t�@�C�����擾
		currentItem = projectDock->currentItem();
	}

	// �����A�v���W�F�N�g�ɑ����Ă�����擾
	CWorkSpaceItem* currentProject
		= currentItem->ancestor(CWorkSpaceItem::Project);

	// �����A�\�����[�V�����ɑ����Ă�����擾
	CWorkSpaceItem* currentSolution
		= currentItem->ancestor(CWorkSpaceItem::Solution);

	if( currentSolution &&
		0 )
	{
		// �\�����[�V�������擾�ł�����\�����[�V�����\�����擾���r���h�����s
		
	}
	else if( currentProject )
	{
		// �v���W�F�N�g���擾�ł�����r���h�����s
		m_compiler->run(currentProject, false);
	}
}

void MainWindow::onDebugSuspend()
{
	// �S�Ẵf�o�b�O�𒆒f
	foreach(CDebugger* debugger, m_debuggers) {
		debugger->suspendDebugging();
	}

	// ���j���[��ύX
	debugResumeAct->setEnabled(true);
	debugSuspendAct->setEnabled(false);
}

void MainWindow::onDebugResume()
{
	// �S�Ẵf�o�b�O���ĊJ
	foreach(CDebugger* debugger, m_debuggers) {
		debugger->resumeDebugging();
	}

	// ���j���[��ύX
	debugResumeAct->setEnabled(false);
	debugSuspendAct->setEnabled(true);
}

void MainWindow::onDebugStop()
{
	// �S�Ẵf�o�b�O���~
	foreach(CDebugger* debugger, m_debuggers) {
		debugger->stopDebugging();
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
			// �N���b�N���Ƀ^�u���A�N�e�B�u�ɏo����悤��
			// ����ێ�
			action->setData(item->uuid().toString());
			// �A�N�e�B�u�ȃ^�u������
			if( index == tabWidget->currentIndex() )
			{
				QFont font = action->font();
				font.setBold(true);
				action->setFont(font);
			}
			// �N���b�N���Ƀ^�u���A�N�e�B�u�ɏo����悤��
			connect(action, SIGNAL(triggered()), this, SLOT(onTabListClicked()));
			// ���j���[�ɒǉ�
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
	// ���j���[�̕\���ʒu���擾
	QWidget* tabButton = tabWidget->cornerWidget(Qt::TopRightCorner);
	QPoint pt;
	for(QWidget* p = tabButton; p; p = p->parentWidget()) {
		pt.setX(pt.x() + p->geometry().x());
		pt.setY(pt.y() + p->geometry().y());
	}
	pt.setY(pt.y() + tabButton->height());
	// ���j���[�\��
	m_tabListMenu->popup(pt);
}

// �^�u�����
void MainWindow::onTabClose()
{
	// ���͂Ƃ�����ۑ�
	onSaveFile();

	// �֘A�t��������
	CDocumentPane * document = static_cast<CDocumentPane*>(tabWidget->currentWidget());

	CWorkSpaceItem * solutionItem = projectDock->currentSolution();
	CWorkSpaceItem * currentItem  = document->assignItem();

	currentItem->setAssignDocument(NULL);

	// �\�����[�V��������������̓v���W�F�N�g�ɖ��O��
	// ���Ă��Ȃ��ꍇ�̓c���[����폜
	if( (solutionItem && solutionItem->isUntitled()) ||
		currentItem->isUntitled() )
	{
		m_workSpace->remove(currentItem);
	}

	// �^�u�����
	tabWidget->removeTab(tabWidget->currentIndex());
}

void MainWindow::buildStart(const QString & filePath)
{
	// �r���h�����J�n

	outputDock->outputCrLf(COutputDock::BuildOutput, tr("Build start"));

	CWorkSpaceItem* targetProjectItem
		= projectDock->currentSolution()->search(filePath);

	// �r���h���J�n�����v���W�F�N�g�Ɋ֘A����t�@�C����S�ĕۑ�
	if( targetProjectItem )
	{
		// �t�@�C����ۑ�
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

	// �v���O���X�o�[��Marquee�X�^�C����
	taskProgress->setRange(0, 0);
	taskProgress->setVisible(true);

	// �o�͂��N���A
	outputDock->clear(COutputDock::BuildOutput);
	messageDock->clear();
}

void MainWindow::buildFinished(bool successed)
{
	// �r���h��������

	taskProgress->setVisible(false);

	if( !successed )
	{
	//	QMessageBox::warning(this, windowTitle(), "erro waitForStarted");

		outputDock->outputCrLf(COutputDock::BuildOutput, tr("Build failed"));
	}
	else
	{
		outputDock->outputCrLf(COutputDock::BuildOutput, tr("Build complete"));
	}
}

// �r���h���̏o�͂��擾
void MainWindow::buildOutput(const QString & output)
{
	CWorkSpaceItem* solutionItem = projectDock->currentSolution();

	QString tmp = QString(output).replace("\r\n", "\n");

	QString outputContents = tmp;

	// uuid���t�@�C�����ɕϊ�

	QRegExp reFileName("\\?(\\{[0-9a-fA-F-]+\\})");

	// uuid���t�@�C�����ɒu��
	if( 0 <= reFileName.indexIn(outputContents) ) {
		QString uuid = reFileName.cap(1);
		CWorkSpaceItem* item = solutionItem ? solutionItem->search(QUuid(uuid)) : NULL;
		if( item ) {
			outputContents.replace(reFileName, item->text());
		}
	}

	// �o�̓h�b�N�ɓ��e�𑗐M
	outputDock->output(COutputDock::BuildOutput, outputContents);

	// ���b�Z�[�W�h�b�N�ɏo�͂�����e�𐶐�

	bool raiseDock = false;

	QRegExp compileError("^((.+)\\(([0-9]+)\\) : (.+))$");
	QRegExp preProcessorError("^#Error:(.+) in line ([0-9]+) \\[(.*)\\]$");
//	QRegExp warning("^(.+):(.+)( �s|at )([0-9]+)\\(.\\[.+\\])$");
	QRegExp excludeInfo("^#(HSP script preprocessor|HSP code generator)");

	for(QStringListIterator ite(tmp.split("\n"));
		ite.hasNext(); )
	{
		QString line = ite.next();
		line.replace("\r", "");

		bool accepted = false;

		for(int i = 0; i < 3 && !accepted; i++)
		{
			CMessageDock::CategoryType type;
			QString fileName, lineNo, desc;
			QUuid uuid;

			switch(i)
			{
			case 0: // �R���p�C���G���[
				if( compileError.indexIn(line) < 0 ) {
					continue;
				}
				fileName = compileError.cap(2);
				lineNo   = compileError.cap(3);
				desc     = compileError.cap(4);
				type     = CMessageDock::ErrorCategory;
				break;
			case 1: // �v���v���Z�X�G���[
				if( preProcessorError.indexIn(line) < 0 ) {
					continue;
				}
				fileName = preProcessorError.cap(3);
				lineNo   = preProcessorError.cap(2);
				desc     = preProcessorError.cap(1);
				type     = CMessageDock::ErrorCategory;
				break;
//			case 2: // �x��
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
			// UUID���t�@�C�����ɕϊ� or �t�@�C������UUID�ɕϊ�
			if( 0 <= reFileName.indexIn(fileName) ) {
				uuid = QUuid(reFileName.cap(1));
				if( CWorkSpaceItem* item
						= solutionItem ? solutionItem->search(uuid)
						               : NULL )
				{
					fileName = item->text();
				}
			} else {
				if( CWorkSpaceItem* item
						= solutionItem ? solutionItem->search(fileName)
						               : NULL )
				{
					uuid = item->uuid();
				}
			}
			// ���b�Z�[�W��ǉ�
			messageDock->addMessage(uuid, fileName, lineNo.toInt(), type, desc);
			raiseDock = true;
			accepted  = true;
		}
	}

	// �h�b�N���B��Ă�����\�ɕ\��
	if( raiseDock )
	{
		QDockWidget* dock = qobject_cast<QDockWidget*>(messageDock->parentWidget());
		dock->raise();
	}
}

void MainWindow::attachedDebugger(CDebugger* debugger)
{
	connect(debugger, SIGNAL(stopAtBreakPoint(const QUuid &,int)), this, SLOT(onGoToLine(const QUuid &,int)));
	connect(debugger, SIGNAL(updateDebugInfo(const QVector<QPair<QString,QString> > &)), this, SLOT(onUpdateDebugInfo(const QVector<QPair<QString,QString> > &)));
	connect(debugger, SIGNAL(updateVarInfo(const QVector<VARIABLE_INFO_TYPE> &)),        this, SLOT(onUpdateVarInfo(const QVector<VARIABLE_INFO_TYPE> &)));
	connect(debugger, SIGNAL(destroyed()), this, SLOT(dettachedDebugger()));

	debugRunAct    ->setVisible( !m_debuggers.empty() );
	noDebugRunAct  ->setVisible( !m_debuggers.empty() );
	debugSuspendAct->setVisible( m_debuggers.empty() );
	debugResumeAct ->setVisible( m_debuggers.empty() );
	debugStopAct   ->setVisible( m_debuggers.empty() );

	if( m_debuggers.empty() ) {
		// �f�o�b�O�J�n
		beginDebugging();
	}

	m_debuggers.insert(debugger);
}

void MainWindow::dettachedDebugger()
{
	CDebugger* debugger = reinterpret_cast<CDebugger*>(sender());

	m_debuggers.remove(debugger);

	debugRunAct    ->setVisible( m_debuggers.empty() );
	noDebugRunAct  ->setVisible( m_debuggers.empty() );
	debugSuspendAct->setVisible( !m_debuggers.empty() );
	debugResumeAct ->setVisible( !m_debuggers.empty() );
	debugStopAct   ->setVisible( !m_debuggers.empty() );

	if( m_debuggers.empty() ) {
		// �f�o�b�O�I��
		endDebugging();
	}
}

bool MainWindow::isDebugging() const
{
	return !m_debuggers.empty();
}

void MainWindow::beginDebugging()
{
	outputDock->clear(COutputDock::DebugOutput);

	// �ŏ��̃f�o�b�K�̏ꍇ�̂݃N���A
	sysInfoDock->clear();
	sysInfoDock->setEnable(true);
	varInfoDock->clear();
	varInfoDock->setEnable(true);

	// ���݂̃��C�A�E�g��ۑ�
	m_stateDefault = saveState();
	// �f�o�b�O���ɂ������݂��Ȃ��h�b�N��ǉ�
	QDockWidget* sysInfoDockWidget = qobject_cast<QDockWidget*>(sysInfoDock->parentWidget());
	QDockWidget* varInfoDockWidget = qobject_cast<QDockWidget*>(varInfoDock->parentWidget());
	QDockWidget* outputDockWidget  = qobject_cast<QDockWidget*>(outputDock->parentWidget());
	addDockWidget(Qt::BottomDockWidgetArea, sysInfoDockWidget);
	addDockWidget(Qt::BottomDockWidgetArea, varInfoDockWidget);
	tabifyDockWidget(outputDockWidget, sysInfoDockWidget);
	tabifyDockWidget(outputDockWidget, varInfoDockWidget);
	sysInfoDockWidget->setVisible(true);
	varInfoDockWidget->setVisible(true);
	// �f�o�b�O�p�̃��C�A�E�g�ɕύX
	restoreState(m_stateDebugging);
}

void MainWindow::endDebugging()
{
	sysInfoDock->setEnable(false);
	varInfoDock->setEnable(false);

	// ���݂̃��C�A�E�g��ۑ�
	m_stateDebugging = saveState();
	// �f�o�b�O���ɂ������݂��Ȃ��h�b�N����菜��
	QDockWidget* sysInfoDockWidget = qobject_cast<QDockWidget*>(sysInfoDock->parentWidget());
	QDockWidget* varInfoDockWidget = qobject_cast<QDockWidget*>(varInfoDock->parentWidget());
	removeDockWidget(sysInfoDockWidget);
	removeDockWidget(varInfoDockWidget);
	sysInfoDockWidget->setVisible(false);
	varInfoDockWidget->setVisible(false);
	// �ʏ�̃��C�A�E�g�ɕύX
	restoreState(m_stateDefault);
}

// �ҏW���ꂽ
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

// �^�u���ύX���ꂽ
void MainWindow::currentTabChanged(int index)
{
	CDocumentPane* document
		= dynamic_cast<CDocumentPane*>(tabWidget->widget(index));

	if( m_lastActivatedDocument )
	{
		CDocumentPane* lastDocument
			= dynamic_cast<CDocumentPane*>(m_lastActivatedDocument);

		// �V�O�i���Ƃ̊֘A�t����ؒf
		disconnect(lastDocument,                       SIGNAL(modificationChanged(bool)));
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
		// �V�O�i���Ɗ֘A�t��
		connect(document,                       SIGNAL(modificationChanged(bool)), saveDocumentAct,    SLOT(setEnabled(bool)));
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

void MainWindow::onSymbolDockVisibilityChanged(bool visible)
{
	if( visible )
	{
		symbolDock->update();
	}
}

