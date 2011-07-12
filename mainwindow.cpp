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

	createDockWindows();
	createStatusBar();
	createMenus();
	createToolBars();
}

void MainWindow::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);
}

void MainWindow::actionTriggered(QAction *action)
{
	qDebug("action '%s' triggered", action->text().toLocal8Bit().data());
}

void MainWindow::createDockWindows()
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

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createMenus()
{
	 QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
//	 menuBar()->
}

void MainWindow::createToolBars()
{
	QToolBar *fileToolBar = addToolBar(tr("File"));

	QAction *newLetterAct;
	newLetterAct = new QAction(QIcon(":/images/new.png"), tr("&New Letter"), this);
	newLetterAct->setShortcuts(QKeySequence::New);
	newLetterAct->setStatusTip(tr("Create a new form letter"));
//	connect(newLetterAct, SIGNAL(triggered()), this, SLOT(newLetter()));
//	fileMenu->addAction(newLetterAct);
	fileToolBar->addAction(newLetterAct);
}
