#include "ui_jumpdialog.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CJumpDialog
	: public QDialog
{
	Q_OBJECT

	Ui::JumpDialog* ui;

	int m_lineNo;

public:

	CJumpDialog(QWidget *parent = 0);

	int lineNo() const;

protected:

public slots:

	void onOk();

private:

};

inline
int CJumpDialog::lineNo() const
{
	return m_lineNo;
}

