#include "ui_newfiledialog.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CNewFileDialog
	: public QDialog
{
	Q_OBJECT

	Ui::NewFileDialog ui;

public:

	CNewFileDialog(QWidget *parent = 0);

protected:


public slots:

private:

};
