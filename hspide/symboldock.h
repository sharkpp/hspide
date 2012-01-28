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
		TypeModule, // モジュール開始位置
		TypeUserSub, // 命令
		TypeUserFunction, // 関数
		TypeLabel,
	} SymbolType;

private:

	Q_OBJECT

	typedef enum {
		NameColumn = 0,
		TypeColumn,
		LineNoColumn,
		FileNameColumn,
		ColumnCount,
		RefIndexColumn = NameColumn,
	} ColumnType;

	typedef struct {
		QUuid      uuid;
		QString    fileName;
		int        lineNo;
		SymbolType type;
		QString    name;
		QString    scope;
	} SymbolInfoType;

	QTreeView* m_listWidget;
	QToolBar* m_toolBar;

	QVector<SymbolInfoType> m_symbolInfo;

public:

	CSymbolDock(QWidget *parent = 0);

	bool clear();
	bool append(const QUuid& uuid, const QString& fileName, int lineNo, const QString& name, const QString& scope, SymbolType type);

	bool analyze(CDocumentPane* document);

protected:

protected slots:

	void onTreeDoubleClicked(const QModelIndex & inde);
	void onTest();

signals:

	void gotoLine(const QUuid & uuid, int lineNo);

};
