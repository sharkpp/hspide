
#include <QMainWindow>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);

protected:
	void showEvent(QShowEvent *event);

public slots:
    void actionTriggered(QAction *action);

private:

	void createDockWindows();
	void createStatusBar();
	void createMenus();
	void createToolBars();

};
