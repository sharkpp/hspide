#include "buildconfigurationspage.h"
#include <QString>
#include <QPushButton>

BuildConfigurationsPage::BuildConfigurationsPage(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::BuildConfigurationsPage)
{
	ui->setupUi(this);
}

