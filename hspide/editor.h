#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QString>
#include "codeedit.h"
#include "projectitem.h"

#pragma once

#ifndef INCLUDE_GUARD_CD203A69_4221_4557_9FC3_4F30394A99DA
#define INCLUDE_GUARD_CD203A69_4221_4557_9FC3_4F30394A99DA

class CEditor
	: public QWidget
{
	Q_OBJECT

	CCodeEdit *		m_editor;
	QString			m_filePath;
	CProjectItem *	m_item;

public:

	CEditor(QWidget *parent = 0);

	// シンボル一覧を指定
	void setSymbols(const QVector<QStringList> & symbols);

	// ファイルから読み込み
	bool load(const QString & filepath);

	// ファイルに保存
	bool save(const QString & filepath = QString());

//	// 一時ファイルに保存
//	bool saveTemporary();
//
//	// 一時ファイルを削除
//	bool clearTemporary();

	// アイテムと関連付け
	bool setAssignItem(CProjectItem * item);
	CProjectItem * assignItem();

	// ファイルパスを取得
	const QString & filePath() const;

	// ファイル名を取得
	QString fileName() const;

	// 空ファイルか？
	bool isNoTitle() const;

protected:

	virtual void resizeEvent(QResizeEvent * event);
	virtual void focusInEvent(QFocusEvent * event);

	void paintLineNumEvent(QPaintEvent * event);
	void mousePressLineNumEvent(QMouseEvent * event);

public slots:

private:

};

#endif // !defined(INCLUDE_GUARD_CD203A69_4221_4557_9FC3_4F30394A99DA)
