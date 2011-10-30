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

	// �V���{���ꗗ���w��
	void setSymbols(const QVector<QStringList> & symbols);

	// �t�@�C������ǂݍ���
	bool load(const QString & filepath);

	// �t�@�C���ɕۑ�
	bool save(const QString & filepath = QString());

//	// �ꎞ�t�@�C���ɕۑ�
//	bool saveTemporary();
//
//	// �ꎞ�t�@�C�����폜
//	bool clearTemporary();

	// �A�C�e���Ɗ֘A�t��
	bool setAssignItem(CProjectItem * item);
	CProjectItem * assignItem();

	// �t�@�C���p�X���擾
	const QString & filePath() const;

	// �t�@�C�������擾
	QString fileName() const;

	// ��t�@�C�����H
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
