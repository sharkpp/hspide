#include <QString>
#include <QPushButton>
#include "buildconfigurationspage.h"
#include "global.h"

BuildConfigurationsPage::BuildConfigurationsPage(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::BuildConfigurationsPage)
{
	ui->setupUi(this);
}

void BuildConfigurationsPage::setBuildConf(const Configuration::BuildConfType& info)
{
	m_infoOrigin = info;
	m_info = info;
	applyInfo();
}

const Configuration::BuildConfType& BuildConfigurationsPage::buildConf()
{
	return m_info;
}

bool BuildConfigurationsPage::isModified() const
{
	return m_infoOrigin != m_info;
}

void BuildConfigurationsPage::onStateChanged(int)
{
	updateInfo();
}

void BuildConfigurationsPage::onTextChanged()
{
	updateInfo();
}

void BuildConfigurationsPage::applyInfo()
{
	struct {
		QWidget* widget;
		QVariant value;
	} targets[] = {
		{ ui->buildConfigurationsPreprocessOnly,	m_info.preprocessOnly	},
		{ ui->buildConfigurationsCompile,			m_info.compile			},
		{ ui->buildConfigurationsMakeExecuteFile,	m_info.make				},
		{ ui->buildConfigurationsNoExecute,			m_info.noExecute		},
		{ ui->buildConfigurationsWithDebug,			m_info.debug			},
	};
	for(int i = 0; i < _countof(targets); i++)
	{
		if( QCheckBox* checkbox = qobject_cast<QCheckBox*>(targets[i].widget) )
		{
			checkbox->blockSignals(true);
			checkbox->setChecked(targets[i].value.toBool());
			checkbox->blockSignals(false);
		}
	}
}

void BuildConfigurationsPage::updateInfo()
{
	m_info.preprocessOnly	= ui->buildConfigurationsPreprocessOnly->isChecked();
	m_info.compile			= ui->buildConfigurationsCompile->isChecked();
	m_info.make				= ui->buildConfigurationsMakeExecuteFile->isChecked();
	m_info.noExecute		= ui->buildConfigurationsNoExecute->isChecked();
	m_info.debug			= ui->buildConfigurationsWithDebug->isChecked();

//	if( isModified() ) {
		emit modified();
//	}
}
