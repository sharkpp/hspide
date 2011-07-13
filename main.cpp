#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QString>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QTranslator trans;

	// メッセージファイルのロード
	trans.load(":/i18n/hspide_ja.qm");
	// translatorを追加
	app.installTranslator(&trans);
//	QString y = QLocale::system().name();
//	QString x = QLocale::languageToString(QLocale::system().language());
	MainWindow mainWin;
	mainWin.resize(800, 600);
	mainWin.show();

	return app.exec();
}
