#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QTranslator trans;

	// メッセージファイルのロード
	trans.load(":/hspide.qm");
	// translatorを追加
	app.installTranslator(&trans);

	MainWindow mainWin;
	mainWin.resize(800, 600);
	mainWin.show();

	return app.exec();
}
