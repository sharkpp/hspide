#include "ui_configdialog.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CConfigDialog
	: public QDialog
{
	Q_OBJECT

	Ui::ConfigDialog ui;

public:

	CConfigDialog(QWidget *parent = 0);

protected:

public slots:

	void onPageChanged(const QModelIndex & index);

private:

};
