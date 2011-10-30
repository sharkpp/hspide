#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QString>

#pragma once

#ifndef INCLUDE_GUARD_3ED30DA5_1A87_433A_9ED9_FCEC987B4EAA
#define INCLUDE_GUARD_3ED30DA5_1A87_433A_9ED9_FCEC987B4EAA

class CCodeEdit
	: public QPlainTextEdit
{
	Q_OBJECT

	friend class CLineNumberArea;
	friend class CRulerArea;

    QWidget *               m_lineNumberWidget;
    QSyntaxHighlighter *    m_highlighter;

    QPalette::ColorRole     m_lineNumberBackgroundColorRole;
    QPalette::ColorRole     m_lineNumberTextColorRole;

    QColor                  m_lineNumberBackgroundColor;
    QColor                  m_lineNumberTextColor;

    bool                    m_visibleLineNumber;

public:

	CCodeEdit(QWidget *parent = 0);

	// ƒVƒ“ƒ{ƒ‹ˆê——‚ðŽw’è
	void setSymbols(const QVector<QStringList> & symbols);

	void setLineNumberBackgroundColorRole(const QPalette::ColorRole & role);
	const QPalette::ColorRole & lineNumberBackgroundColorRole() const;

	void setLineNumberTextColorRole(const QPalette::ColorRole & role);
	const QPalette::ColorRole & lineNumberTextColorRole() const;

	void setLineNumberBackgroundColor(const QColor & color);
	const QColor & lineNumberBackgroundColor() const;

	void setLineNumberTextColor(const QColor & color);
	const QColor & lineNumberTextColor() const;

	void setLineNumberVisible(bool visible);
	bool isLineNumberVisible() const;

protected:

	virtual void resizeEvent(QResizeEvent * event);

	void paintLineNumEvent(QPaintEvent * event);
	void paintRulerEvent(QPaintEvent * event);
	void mousePressLineNumEvent(QMouseEvent * event);

public slots:

	void updateLineNumberWidth(int newBlockCount);
	void updateLineNumber(const QRect & rect ,int dy);

private:

	int lineNumberWidth();

};

inline const QPalette::ColorRole & CCodeEdit::lineNumberBackgroundColorRole() const {
	return m_lineNumberBackgroundColorRole;
}

inline const QPalette::ColorRole & CCodeEdit::lineNumberTextColorRole() const {
	return m_lineNumberTextColorRole;
}

inline const QColor & CCodeEdit::lineNumberBackgroundColor() const {
	return
		QPalette::NoRole == m_lineNumberBackgroundColorRole
			? m_lineNumberBackgroundColor
			: palette().color(m_lineNumberBackgroundColorRole);
}

inline const QColor & CCodeEdit::lineNumberTextColor() const {
	return
		QPalette::NoRole == m_lineNumberTextColorRole
			? m_lineNumberTextColor
			: palette().color(m_lineNumberTextColorRole);
}

inline bool CCodeEdit::isLineNumberVisible() const {
	return m_visibleLineNumber;
}

#endif // !defined(INCLUDE_GUARD_3ED30DA5_1A87_433A_9ED9_FCEC987B4EAA)
