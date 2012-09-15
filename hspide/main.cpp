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

	// �߂�
	//QSettings �ł́@�W���� Windows �̓��W�X�g���ɏ������܂��
	//HKEY_CURRENT_USER\Software\�\�t�g�E�G�A��\�L�[
	//�ŏ������܂��B
	//QSettings settings("vzexfiler.ini", QSettings::IniFormat);
	//�ƃt�@�C������ .ini ��QSettings::IniFormat���w�肵�Ă��ƁA�t�@�C���ɏ������ނ悤�ɂȂ�B

	// ���b�Z�[�W�t�@�C���̃��[�h
	trans.load(QString(":/i18n/hspide_ja.pm"));
	// translator��ǉ�
	app.installTranslator(&trans);
//	QString y = QLocale::system().name();
//	QString x = QLocale::languageToString(QLocale::system().language());
	MainWindow mainWin;
	mainWin.show();

	return app.exec();
}
