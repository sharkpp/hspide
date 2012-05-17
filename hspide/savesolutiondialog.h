#include "ui_savesolutiondialog.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CWorkSpaceItem;
class QStandardItem;

class CSaveSolutionDialog
	: public QDialog
{
	Q_OBJECT

	Ui::SaveSolutionDialog ui;

	QList<QStandardItem*> m_items;
	QList<CWorkSpaceItem*> m_checkedItems;

	enum {
		FileNameColumn,
		SaveCheckColumn,
		ColumnCount,
	};

public:

	CSaveSolutionDialog(QWidget *parent = 0);

	bool setSolution(CWorkSpaceItem* item);

	QList<CWorkSpaceItem*> list();

protected:

public slots:

	void clicked(QAbstractButton* button);
	void canceled();

private:

};
