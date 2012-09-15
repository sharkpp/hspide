#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QString>
#include <QTextCodec>

void myMessageHandler(QtMsgType type, const char *msg)
 {
	 FILE* fp = fopen("hspide.log", "ab");
     switch (type) {
     case QtDebugMsg:
         fprintf(fp, "Debug: %s\r\n", msg);
         break;
     case QtWarningMsg:
         fprintf(fp, "Warning: %s\r\n", msg);
         break;
     case QtCriticalMsg:
         fprintf(fp, "Critical: %s\r\n", msg);
         break;
     case QtFatalMsg:
         fprintf(fp, "Fatal: %s\r\n", msg);
         abort();
     }
	 fclose(fp);
 }

int main(int argc, char **argv)
{
#if 0 && !defined(_DEBUG)
	qInstallMsgHandler(myMessageHandler);
#endif

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
	mainWin.show();

	return app.exec();
}
