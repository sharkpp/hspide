#include "ui_configdialog.h"
#include "configuration.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CConfigDialog
	: public QDialog
{
	Q_OBJECT

	Ui::ConfigDialog ui;

	Configuration m_configuration;

public:

	CConfigDialog(QWidget *parent = 0);

	void setConfiguration(const Configuration& info);
	const Configuration& configuration() const;

protected:

public slots:

	void onOk();
	void onPageChanged(const QModelIndex & index);

private:

};
