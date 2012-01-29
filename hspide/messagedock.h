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
	};

	typedef struct {
		CategoryType category;
		QString	description;
		QUuid	uuid;
		QString	fileName;
		int		lineNo;
	} MessageInfoType;

	QVector<MessageInfoType> m_messages;

	QTreeView* m_listWidget;
	QToolBar*  m_toolBar;

public:

	CMessageDock(QWidget *parent = 0);

	void clear();
	void addMessage(const QUuid & uuid, const QString & fileName, int lineNo, CategoryType category, const QString & description);

protected:


private:

protected slots:

	void doubleClickedList(const QModelIndex & inde);

	void onVisibledError();
	void onVisibledWarning();
	void onVisibledInfomation();

signals:

	void gotoLine(const QUuid & uuid, int lineNo);

};
