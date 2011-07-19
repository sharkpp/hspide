#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QString>
#include <QTextCodec>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	QTranslator trans;

	qApp->setApplicationName("HSP script editor");

	// めも
	//QSettings では　標準で Windows はレジストリに書き込まれる
	//HKEY_CURRENT_USER\Software\ソフトウエア名\キー
	//で書き込まれる。
	//QSettings settings("vzexfiler.ini", QSettings::IniFormat);
	//とファイル名を .ini でQSettings::IniFormatを指定してやると、ファイルに書き込むようになる。

	// メッセージファイルのロード
	trans.load(QString(":/i18n/hspide_ja.pm"));
	// translatorを追加
	app.installTranslator(&trans);
//	QString y = QLocale::system().name();
//	QString x = QLocale::languageToString(QLocale::system().language());
	MainWindow mainWin;
	mainWin.resize(800, 600);
	mainWin.show();

	return app.exec();
}
