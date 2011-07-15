#include "mainwindow.h"
#include <QAction>
#include <QDockWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QTabWidget>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	setObjectName("MainWindow");
	setWindowTitle("HSP script editor");

//	QTextEdit * textEdit = new QTextEdit;
//	setCentralWidget(textEdit);
	QTabWidget * tabWidget = new QTabWidget(this);
//	QTabBar * tabBar = new QTabBar(this);
//	tabWidget->setTabBar(tabBar);
//	tabWidget->setElideMode(Qt::ElideRight);
	for(int i = 0; i < 5; i++) {
	tabWidget->addTab(new QTextEdit, "test");
	}
	setCentralWidget(tabWidget);

	setupDockWindows();
	setupStatusBar();
	setupToolBars();
	setupActions();
}

void MainWindow::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);
}

void MainWindow::actionTriggered(QAction *action)
{
	qDebug("action '%s' triggered", action->text().toLocal8Bit().data());
}

void MainWindow::setupDockWindows()
{
	QDockWidget * dock;
	dock = new QDockWidget("test", this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	QListWidget *customerList = new QListWidget(dock);
	dock->setWidget(customerList);
	addDockWidget(Qt::RightDockWidgetArea, dock);

	dock = new QDockWidget("test2", this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	QListWidget *customerList2 = new QListWidget(dock);
	dock->setWidget(customerList2);
	addDockWidget(Qt::BottomDockWidgetArea, dock);
}

void MainWindow::setupStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::setupToolBars()
{
	m_generalToolbar = addToolBar(tr("General"));
}

void MainWindow::setupActions()
{
	QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
	QMenu * editMenu = menuBar()->addMenu(tr("&Edit"));

//	connect(newLetterAct, SIGNAL(triggered()), this, SLOT(newLetter()));

	QAction *newDocumentAct = new QAction(QIcon(":/images/tango/document-new.png"), tr("&New"), this);
	         newDocumentAct->setShortcuts(QKeySequence::New);
	         newDocumentAct->setStatusTip(tr("Create a new file"));
	fileMenu->addAction(newDocumentAct);
	m_generalToolbar->addAction(newDocumentAct);

	QAction *openDocumentAct = new QAction(QIcon(":/images/tango/document-open.png"), tr("&Open"), this);
	         openDocumentAct->setShortcuts(QKeySequence::Open);
	         openDocumentAct->setStatusTip(tr("Open the file"));
	fileMenu->addAction(openDocumentAct);
	m_generalToolbar->addAction(openDocumentAct);

	fileMenu->addSeparator();

	QAction *saveDocumentAct = new QAction(QIcon(":/images/tango/document-save.png"), tr("&Save"), this);
	         saveDocumentAct->setShortcuts(QKeySequence::Save);
	         saveDocumentAct->setStatusTip(tr("Save the file"));
	fileMenu->addAction(saveDocumentAct);
	m_generalToolbar->addAction(saveDocumentAct);

	QAction *saveAsDocumentAct = new QAction(QIcon(":/images/tango/document-save-as.png"), tr("S&ave as"), this);
	         saveAsDocumentAct->setShortcuts(QKeySequence::SaveAs);
	         saveAsDocumentAct->setStatusTip(tr("Save the file with a name"));
	fileMenu->addAction(saveAsDocumentAct);
//	m_generalToolbar->addAction(saveAsDocumentAct);

	m_generalToolbar->addSeparator();

//	QAction *saveDocumentAct = new QAction(QIcon(":/images/tango/document-save.png"), tr("&Save"), this);
//	         saveDocumentAct->setShortcuts(QKeySequence::Save);
//	         saveDocumentAct->setStatusTip(tr("Save the file"));
//	editMenu->addAction(saveDocumentAct);
//	m_generalToolbar->addAction(saveDocumentAct);
	
}
