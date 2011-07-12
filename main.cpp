#include "mainwindow.h"
#include <QApplication>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	MainWindow mainWin;
	mainWin.resize(800, 600);
	mainWin.show();
	return app.exec();
}
