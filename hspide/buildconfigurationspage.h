#include "ui_buildconfigurationspage.h"
#include "configuration.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class BuildConfigurationsPage
	: public QWidget
{
	Q_OBJECT

	Ui::BuildConfigurationsPage* ui;

	Configuration::BuildConfType m_infoOrigin;
	Configuration::BuildConfType m_info;

public:

	BuildConfigurationsPage(QWidget *parent = 0);

	void setBuildConf(const Configuration::BuildConfType& info);
	const Configuration::BuildConfType& buildConf();

	bool isModified() const;

private:

	void applyInfo();
	void updateInfo();

public slots:

	void onStateChanged(int);
	void onTextChanged();

signals:

	void modified(); 

};
