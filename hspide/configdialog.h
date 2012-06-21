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
		KeyAssignListShortcutKeyColumn,
		KeyAssignListColumnNum,
	};

	Ui::ConfigDialog ui;

	Configuration m_configuration;

	Configuration::ColorMetricsEnum     m_lastIndexOfColorMetrics;
	Configuration::ColorMetricsInfoType m_currentColorMetricsInfo;

public:

	CConfigDialog(QWidget *parent = 0);

	const Configuration& configuration() const;

protected:

	void setConfiguration(const Configuration& info);

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
	void onKeyAsignPresetChanged(int);
	void onKeyAsignPresetRemove();
	void onKeyAsignPresetSave();
	void onToolbarPresetChanged(int);
	void onToolbarPresetSave();
	void onToolbarPresetRemove();
	void onToolbarAppendButton();
	void onToolbarAppendSeparator();
	void onToolbarRemoveButton();
	void onToolbarButtonGoTop();
	void onToolbarButtonGoBottom();

private:

};
