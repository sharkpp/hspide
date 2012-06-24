#include "ui_savepresetdialog.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CSavePresetDialog
	: public QDialog
{
	Q_OBJECT

	Ui::SavePresetDialog ui;

	QString m_newTitle;
	bool m_newTitlePresent;

public:

	CSavePresetDialog(QWidget *parent = 0);

	void setNewTitlePresent(bool newTitlePresent);
	bool newTitlePresent() const;
	void setNewTitle(const QString& newTitle);
	const QString& newTitle() const;

protected:

public slots:

	void onOk();
	void onEnablePresetNameUI(bool checked);
	void onChangedPresetName(const QString& text);

private:

};
