#include <QTreeView>
#include <QVector>
#include <QUuid>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CMessageDock
	: public QWidget
{
	Q_OBJECT

	typedef enum {
		CategoryColumn = 0,
		DescriptionColumn,
		FileColumn,
		LineColumn,
		ColumnCount,
	};

	typedef struct {
		QString	description;
		QUuid	uuid;
		QString	fileName;
		int		lineNo;
	} MessageInfoType;

	QTreeView * listWidget;

	QVector<MessageInfoType> m_messages;

public:

	CMessageDock(QWidget *parent = 0);

	void clear();
	void addMessage(const QUuid & uuid, const QString & fileName, int lineNo, const QString & description);

protected:

	virtual void resizeEvent(QResizeEvent * event);

private:

protected slots:

	void doubleClickedList(const QModelIndex & inde);

signals:

//	void oepnItem(CWorkSpaceItem * item);
	void gotoLine(const QUuid & uuid, int lineNo);

};
