#include "configuration.h"
#include "ui_configdialog.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CConfigDialog
	: public QDialog
{
	Q_OBJECT

	typedef enum {
		BuildTargetListName = 0,
		BuildTargetListDebug,
		BuildTargetListObjectOnly,
		BuildTargetListColumnNum,
	};

	typedef enum {
		ColorListCategoryColumn = 0,
		ColorListEnableColumn,
		ColorListFontBoldColumn,
        ColorListForegroundColorColumn,
		ColorListBackgroundColorColumn,
		ColorListColumnNum,
	};

	typedef enum {
		KeyAssignListNameColumn = 0,
		KeyAssignListKeyAssignKeyColumn,
		KeyAssignListColumnNum,
	};

	Ui::ConfigDialog* ui;

	Configuration m_configuration;
	QVector<Configuration::KeyAssignInfoType> m_currentKeyAssign;

    Configuration::ColorMetricsEnum     m_lastIndexOfColorMetrics;
//	Configuration::ColorMetricsInfoType m_currentColorMetricsInfo;

public:

	CConfigDialog(QWidget *parent = 0);

	const Configuration& configuration() const;

protected:

	void setConfiguration(const Configuration* conf);

	void updateBuildConf(const Configuration::BuildConfType& buildConf);
	void applyBuildConf(Configuration::BuildConfType& buildConf);
	void updateBuildConfPresetList();

	void updateKeyAssign(const QVector<Configuration::KeyAssignInfoType>& keyAssign);
	void applyKeyAssign(QVector<Configuration::KeyAssignInfoType>& keyAssign);
	void updateKeyAssignPresetList();

	void updateToolbar(const QVector<Configuration::ActionEnum>& toolbar);
	void applyToolbar(QVector<Configuration::ActionEnum>& toolbar);
	void updateToolbarPresetList();
	void checkUpdateToolbarPresetList();

    void updateMetrics(int index);

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
	void onBuildConfPresetChanged(int);
	void onBuildConfPresetRemove();
	void onBuildConfPresetSave();
	void onBuildConfModified();
	void onKeyAssignChanged(QTreeWidgetItem * item, int column);
	void onKeyAssignPresetChanged(int);
	void onKeyAssignPresetRemove();
	void onKeyAssignPresetSave();
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
