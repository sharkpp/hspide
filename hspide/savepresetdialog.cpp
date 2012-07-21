#include "savepresetdialog.h"
#include <QString>
#include <QPushButton>

CSavePresetDialog::CSavePresetDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::SavePresetDialog)
	, m_newTitlePresent(false)
{
	ui->setupUi(this);
}

void CSavePresetDialog::setNewTitlePresent(bool newTitlePresent)
{
	m_newTitlePresent = newTitlePresent;
	ui->newTitlePresent->setChecked(m_newTitlePresent ? Qt::Checked : Qt::Unchecked);
	ui->newTitle->setDisabled(!m_newTitlePresent);
	ui->newTitle->setFocus(Qt::OtherFocusReason);
}

bool CSavePresetDialog::newTitlePresent() const
{
	return m_newTitlePresent;
}

void CSavePresetDialog::setNewTitle(const QString& newTitle)
{
	m_newTitle = newTitle;
	ui->newTitle->setText(m_newTitle);
}

const QString& CSavePresetDialog::newTitle() const
{
	return m_newTitle;
}

void CSavePresetDialog::onOk()
{
	m_newTitlePresent = Qt::Checked == ui->newTitlePresent->checkState();
	m_newTitle = ui->newTitle->text();

	accept();
}

void CSavePresetDialog::onEnablePresetNameUI(bool checked)
{
	ui->newTitle->setDisabled(!checked);
	onChangedPresetName(ui->newTitle->text());
}

void CSavePresetDialog::onChangedPresetName(const QString& text)
{
	ui->buttonBox->button(QDialogButtonBox::Yes)
		->setDisabled(text.isEmpty() && Qt::Checked == ui->newTitlePresent->checkState());
}
