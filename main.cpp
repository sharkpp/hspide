#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QTranslator trans;

	// ���b�Z�[�W�t�@�C���̃��[�h
	trans.load(":/hspide.qm");
	// translator��ǉ�
	app.installTranslator(&trans);

	MainWindow mainWin;
	mainWin.resize(800, 600);
	mainWin.show();

	return app.exec();
}
