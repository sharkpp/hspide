#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QString>
#include "codeedit.h"
#include "workspaceitem.h"

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef INCLUDE_GUARD_CD203A69_4221_4557_9FC3_4F30394A99DA
#define INCLUDE_GUARD_CD203A69_4221_4557_9FC3_4F30394A99DA

class CDocumentPane
	: public QWidget
{
	Q_OBJECT

	CCodeEdit *		m_editor;
	QString			m_filePath;
	CWorkSpaceItem*	m_item;

public:

	CDocumentPane(QWidget *parent = 0);

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
	bool setAssignItem(CWorkSpaceItem * item);
	CWorkSpaceItem * assignItem();

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