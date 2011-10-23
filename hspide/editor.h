#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QString>
#include "codeedit.h"

#pragma once

#ifndef INCLUDE_GUARD_CD203A69_4221_4557_9FC3_4F30394A99DA
#define INCLUDE_GUARD_CD203A69_4221_4557_9FC3_4F30394A99DA

class CEditor
	: public QWidget
{
	Q_OBJECT

	CCodeEdit * mEditor;

	QString	mFilePath;

public:

	CEditor(QWidget *parent = 0);

	// �t�@�C������ǂݍ���
	bool load(const QString & filepath);

	// �t�@�C���ɕۑ�
	bool save(const QString & filepath = QString());

//	// �ꎞ�t�@�C���ɕۑ�
//	bool saveTemporary();
//
//	// �ꎞ�t�@�C�����폜
//	bool clearTemporary();

	// �t�@�C���p�X���擾
	const QString & filePath() const;

	// �t�@�C�������擾
	QString fileName() const;

	// ��t�@�C�����H
	bool isNoTitle() const;

protected:

	virtual void resizeEvent(QResizeEvent * event);

	void paintLineNumEvent(QPaintEvent * event);
	void mousePressLineNumEvent(QMouseEvent * event);

public slots:

private:

};

#endif // !defined(INCLUDE_GUARD_CD203A69_4221_4557_9FC3_4F30394A99DA)
