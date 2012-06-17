#include <QTreeView>
#include <QToolBar>
#include <QVector>
#include <QUuid>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CMessageDock
	: public QWidget
{
public:

	typedef enum {
		NoneCategory = 0,
		InfomationCategory,
		WarningCategory,
		ErrorCategory,
	} CategoryType;

private:

	Q_OBJECT

	typedef enum {
		CategoryColumn = 0,
		FileNameColumn,
		LineNoColumn,
		DescriptionColumn,
		ColumnCount,
		RefIndexColumn = CategoryColumn,
	};

	typedef struct {
		CategoryType category;
		QString	description;
		QUuid	uuid;
		int		lineNo;
	} MessageInfoType;

	QVector<MessageInfoType> m_messages;

	QTreeView* m_listWidget;
	QToolBar*  m_toolBar;

	QAction*   m_visibledErrorAction;
	QAction*   m_visibledWarningAction;
	QAction*   m_visibledInfomationAction;

	int        m_countOfErrorMessages;
	int        m_countOfWarningMessages;
	int        m_countOfInfomationMessages;

public:

	CMessageDock(QWidget *parent = 0);

	void clear();
	void addMessage(const QUuid& uuid, int lineNo, CategoryType category, const QString& description);

protected:

	void updateMessagesCount();

private:

protected slots:

	void doubleClickedList(const QModelIndex& inde);

	void onUpdateCategoryVisibled();

signals:

	void gotoLine(const QUuid& uuid, int lineNo);

};
