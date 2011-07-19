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

	qApp->setApplicationName( "unifiler" );

	// ���b�Z�[�W�t�@�C���̃��[�h
	trans.load(QString(":/i18n/hspide_ja.pm"));
	// translator��ǉ�
	app.installTranslator(&trans);
//	QString y = QLocale::system().name();
//	QString x = QLocale::languageToString(QLocale::system().language());
	MainWindow mainWin;
	mainWin.resize(800, 600);
	mainWin.show();

	return app.exec();
}
