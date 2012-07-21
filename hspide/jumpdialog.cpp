#include "jumpdialog.h"
#include <QString>

CJumpDialog::CJumpDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::JumpDialog)
	, m_lineNo(-1)
{
	ui->setupUi(this);
}

void CJumpDialog::onOk()
{
	m_lineNo = ui->lineNumber->text().toInt();

	accept();
}

