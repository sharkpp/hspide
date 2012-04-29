#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QString>
#include "codeedit.h"
#include "workspaceitem.h"
#include "configuration.h"
#include "hsp3highlighter.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef INCLUDE_GUARD_CD203A69_4221_4557_9FC3_4F30394A99DA
#define INCLUDE_GUARD_CD203A69_4221_4557_9FC3_4F30394A99DA

class CDocumentPane
	: public QWidget
{
	Q_OBJECT

	CCodeEdit *			m_editorWidget;
	CHsp3Highlighter*	m_highlighter;

	QString				m_filePath;

	CWorkSpaceItem*		m_assignItem;

public:

	CDocumentPane(QWidget *parent = 0);

	CCodeEdit * editor();

	// 設定更新
	void setConfiguration(const Configuration& info);

	// シンボル一覧を指定
	void setSymbols(const QVector<QStringList> & symbols);

	// ファイルから読み込み
	bool load(const QString & filepath, const QString & tmplFilePath = QString());

	// ファイルに保存
	bool save(const QString & filepath = QString());

//	// 一時ファイルに保存
//	bool saveTemporary();
//
//	// 一時ファイルを削除
//	bool clearTemporary();

	// 行に移動
	bool jump(int lineNo);

	// アイテムと関連付け
	bool setAssignItem(CWorkSpaceItem * item);
	CWorkSpaceItem * assignItem();

	// ファイルパスを取得
	const QString & filePath() const;

	// ファイル名を取得
	QString fileName() const;

	// 空ファイルか？
	bool isUntitled() const;

	// 変更されているか
	bool isModified() const;

protected:

	virtual void resizeEvent(QResizeEvent * event);
	virtual void focusInEvent(QFocusEvent * event);

	void paintLineNumEvent(QPaintEvent * event);
	void mousePressLineNumEvent(QMouseEvent * event);

public slots:

	void updateConfiguration(const Configuration& info);

	void onModificationChanged(bool changed);
	void onPressEditorIconArea(int lineNo);

signals:
	void modificationChanged(bool changed);
	void updateBreakpoint();

private:

};

inline
CCodeEdit * CDocumentPane::editor()
{
	return m_editorWidget;
}

#endif // !defined(INCLUDE_GUARD_CD203A69_4221_4557_9FC3_4F30394A99DA)
