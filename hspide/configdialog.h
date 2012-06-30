#include "ui_configdialog.h"
#include "configuration.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CConfigDialog
	: public QDialog
{
	Q_OBJECT

	typedef enum {
		ColorListCategoryColumn = 0,
		ColorListEnableColumn,
		ColorListFontBoldColumn,
		ColorListForegroundColorCount,
		ColorListBackgroundColorColumn,
		ColorListColumnNum,
	};

	typedef enum {
		KeyAssignListNameColumn = 0,
		KeyAssignListKeyAssignKeyColumn,
		KeyAssignListColumnNum,
	};

	Ui::ConfigDialog ui;

	Configuration m_configuration;
	QVector<Configuration::KeyAssignInfoType> m_currentKeyAssign;

	Configuration::ColorMetricsEnum     m_lastIndexOfColorMetrics;
	Configuration::ColorMetricsInfoType m_currentColorMetricsInfo;

public:

	CConfigDialog(QWidget *parent = 0);

	const Configuration& configuration() const;

protected:

	void setConfiguration(const Configuration& info);

	void updateKeyAssign(const QVector<Configuration::KeyAssignInfoType>& keyAssign);
	void applyKeyAssign(QVector<Configuration::KeyAssignInfoType>& keyAssign);
	void updateKeyAssignPresetList();

	void updateToolbar(const QVector<Configuration::ActionEnum>& toolbar);
	void applyToolbar(QVector<Configuration::ActionEnum>& toolbar);
	void updateToolbarPresetList();
	void checkUpdateToolbarPresetList();

public slots:

	void onOk();
	void onPageChanged(const QModelIndex& index);
	void onClickedInstallDirectoryRef();
	void onClickedCommonDirectoryRef();
	void onCurrentMetricsChanged();
	void onMetricsChanged(QTreeWidgetItem* item);
	void onChangedMetricsEnable(int);
	void onChangedMetricsFontBold(int);
	void onChangedMetricsBgcolor();
	void onChangedMetricsFgcolor();
	void onKeyAssignChanged(QTreeWidgetItem * item, int column);
	void onKeyAsignPresetChanged(int);
	void onKeyAsignPresetRemove();
	void onKeyAsignPresetSave();
	void onToolbarPresetChanged(int);
	void onToolbarPresetSave();
	void onToolbarPresetRemove();
	void onToolbarAppendButton();
	void onToolbarAppendButtonLast();
	void onToolbarRemoveButton();
	void onToolbarButtonGoUp();
	void onToolbarButtonGoDown();

private:

};
