#include "ui_savesolutiondialog.h"
#include "workspaceitem.h"
#include <QItemEditorCreatorBase>
#include <QComboBox>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CWorkSpaceItem;
class QTreeWidgetItem;

class CSaveSolutionDialog
	: public QDialog
{
public:

	typedef QPair<CWorkSpaceItem*, CWorkSpaceItem::SaveType> SavingItemInfo;

private:

	Q_OBJECT

	Ui::SaveSolutionDialog ui;

	QList<QTreeWidgetItem*> m_items;
	QList<SavingItemInfo> m_savingItems;

public:

	CSaveSolutionDialog(QWidget *parent = 0);

	bool setSolution(CWorkSpaceItem* item);

	QList<SavingItemInfo> list();

protected:

public slots:

	void clicked(QAbstractButton* button);
	void canceled();

private:

};
