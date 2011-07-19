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
	mainWin.resize(800, 600);
	mainWin.show();

	return app.exec();
}
