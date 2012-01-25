#include <QTreeView>
#include <QToolBar>
#include <QUuid>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CDocumentPane;

class CSymbolDock
	: public QWidget
{
public:

	typedef enum {
		TypeUnknown = 0,
		TypeUserSub, // –½—ß
		TypeUserFunction, // ŠÖ”
		TypeLabel,
	} SymbolType;

private:

	Q_OBJECT

	typedef enum {
		FileNameColumn = 0,
		LineNoColumn,
		TypeColumn,
		DescriptionColumn,
		ColumnCount,
	} ColumnType;

	typedef struct {
		QUuid      uuid;
		QString    fileName;
		int        lineNo;
		SymbolType type;
		QString    description;
	} SymbolInfoType;

	QTreeView* listWidget;

	QToolBar* m_toolBar;

	QVector<SymbolInfoType> m_symbolInfo;

public:

	CSymbolDock(QWidget *parent = 0);

	bool clear();
	bool append(const QUuid& uuid, const QString& fileName, int lineNo, SymbolType type, const QString& description);

	bool analyze(CDocumentPane* document);

protected:

	virtual void resizeEvent(QResizeEvent * event);

protected slots:

	void doubleClickedList(const QModelIndex & inde);

signals:

	void gotoLine(const QUuid & uuid, int lineNo);

};
