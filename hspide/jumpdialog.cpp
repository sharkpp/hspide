#include "jumpdialog.h"
#include <QString>

CJumpDialog::CJumpDialog(QWidget *parent)
	: QDialog(parent)
	, m_lineNo(-1)
{
	ui.setupUi(this);
}

void CJumpDialog::onOk()
{
	m_lineNo = ui.lineNumber->text().toInt();

	accept();
}

