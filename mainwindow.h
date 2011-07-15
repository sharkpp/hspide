
#include <QMainWindow>

class MainWindow : public QMainWindow
{
	Q_OBJECT

	QToolBar *	m_generalToolbar;

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
	void setupActions();

//	void newAction();
};
