#include <QTreeView>
#include <QToolBar>
#include <QTimer>
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

	QVector<SymbolInfoType> m_symbolInfo;

	QTreeView*     m_listWidget;
	QToolBar*      m_toolBar;

	CDocumentPane* m_document;

	QTimer*        m_lazyUpdateTimer;

public:

	CSymbolDock(QWidget *parent = 0);

	// シンボル一覧をクリア
	void clear();

	// 更新
	void update();

	// シンボルを追加
	void append(const QUuid& uuid, const QString& fileName, int lineNo, const QString& name, const QString& scope, SymbolType type);

	// シンボルを解析
	void analyze(CDocumentPane* document);

	// ドキュメントと関連付け
	void setAssignDocument(CDocumentPane* document);

protected:

protected slots:

	void onTreeDoubleClicked(const QModelIndex & inde);
	void onTest();

	void onDocumentUpdate();

signals:

	void gotoLine(const QUuid & uuid, int lineNo);

};
