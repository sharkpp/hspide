#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QString>
#include <QUuid>
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

	CCodeEdit*			m_editorWidget;
	CHsp3Highlighter*	m_highlighter;

	QUuid				m_uuid;

	CWorkSpaceItem*		m_assignItem;

	int					m_breakPointIcon;
	int					m_cursorIcon;

public:

	CDocumentPane(QWidget *parent = 0);

	CCodeEdit* editor();

	// �V���{���ꗗ���w��
	void setSymbols(const QVector<QStringList>& symbols);

	// �t�@�C������ǂݍ���
	bool load(const QString& filepath, const QString& tmplFilePath = QString());

	// �t�@�C���ɕۑ�
	bool save(const QString& filepath = QString(), bool saveAs = false);

//	// �ꎞ�t�@�C���ɕۑ�
//	bool saveTemporary();
//
//	// �ꎞ�t�@�C�����폜
//	bool clearTemporary();

	// �s�Ɉړ�
	bool jump(int lineNo);

	// �A�C�e���Ɗ֘A�t��
	bool setAssignItem(CWorkSpaceItem* item);
	CWorkSpaceItem* assignItem();

	// uuid���擾
	const QUuid& uuid() const;

	// �t�@�C���p�X���擾
	QString path() const;

	// �^�C�g�����擾
	QString title() const;

	// ����t�@�C��(�f�B�X�N�ɕۑ����Ă��Ȃ��t�@�C��)���H
	bool isUntitled() const;

	// �ύX����Ă��邩
	bool isModified() const;

	void markCursorLine(const QUuid& uuid, int lineNo);
	void unmarkCursorLine();

protected:

	virtual void resizeEvent(QResizeEvent* event);
	virtual void focusInEvent(QFocusEvent* event);

	void paintLineNumEvent(QPaintEvent* event);
	void mousePressLineNumEvent(QMouseEvent* event);

public slots:

	void updateConfiguration(const Configuration& info);

	void onModificationChanged(bool changed);
	void onPressEditorIconArea(int lineNo);

	void onBreakPointChanged(const QUuid& uuid, const QList<QPair<int, bool> >& modifiedLineNumberes);

signals:

	void modificationChanged(bool changed);
	void updateBreakpoint();

private:

};

inline
CCodeEdit* CDocumentPane::editor()
{
	return m_editorWidget;
}

#endif // !defined(INCLUDE_GUARD_CD203A69_4221_4557_9FC3_4F30394A99DA)
