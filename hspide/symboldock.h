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
		TypeModule, // ���W���[���J�n�ʒu
		TypeUserSub, // ����
		TypeUserFunction, // �֐�
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

	// �V���{���ꗗ���N���A
	void clear();

	// �X�V
	void update();

	// �V���{����ǉ�
	void append(const QUuid& uuid, const QString& fileName, int lineNo, const QString& name, const QString& scope, SymbolType type);

	// �V���{�������
	void analyze(CDocumentPane* document);

	// �h�L�������g�Ɗ֘A�t��
	void setAssignDocument(CDocumentPane* document);

protected:

protected slots:

	void onTreeDoubleClicked(const QModelIndex & inde);
	void onTest();

	void onDocumentUpdate();

signals:

	void gotoLine(const QUuid & uuid, int lineNo);

};
