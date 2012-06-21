#include "savepresetdialog.h"
#include <QString>

CSavePresetDialog::CSavePresetDialog(QWidget *parent)
	: QDialog(parent)
	, m_newTitlePresent(false)
{
	ui.setupUi(this);
}

void CSavePresetDialog::setNewTitlePresent(bool newTitlePresent)
{
	m_newTitlePresent = newTitlePresent;
	ui.newTitlePresent->setChecked(m_newTitlePresent ? Qt::Checked : Qt::Unchecked);
	ui.newTitle->setDisabled(!m_newTitlePresent);
}

bool CSavePresetDialog::newTitlePresent() const
{
	return m_newTitlePresent;
}

void CSavePresetDialog::setNewTitle(const QString& newTitle)
{
	m_newTitle = newTitle;
	ui.newTitle->setText(m_newTitle);
}

const QString& CSavePresetDialog::newTitle() const
{
	return m_newTitle;
}

void CSavePresetDialog::onOk()
{
	m_newTitlePresent = Qt::Checked == ui.newTitlePresent->checkState();
	m_newTitle = ui.newTitle->text();

	accept();
}

