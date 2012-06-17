#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QCompleter>
#include <QString>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef INCLUDE_GUARD_3ED30DA5_1A87_433A_9ED9_FCEC987B4EAA
#define INCLUDE_GUARD_3ED30DA5_1A87_433A_9ED9_FCEC987B4EAA

class CCodeEdit
	: public QPlainTextEdit
{
	Q_OBJECT

	friend class CLineNumberArea;
	friend class CRulerArea;

	QWidget*            m_lineNumberWidget;
	QCompleter*         m_completer;

	QPalette::ColorRole m_lineIconBackgroundColorRole;
	QPalette::ColorRole m_lineNumberBackgroundColorRole;
	QPalette::ColorRole m_lineNumberTextColorRole;

	QColor              m_lineIconBackgroundColor;
	QColor              m_lineNumberBackgroundColor;
	QColor              m_lineNumberTextColor;

	bool                m_visibleLineNumber;

	QSize               m_lineIconSize;
	QMap<int, QIcon>    m_lineIconMap;

	int                 m_tabStopCharWidth;

public:

	CCodeEdit(QWidget *parent = 0);

	// ƒVƒ“ƒ{ƒ‹ˆê——‚ðŽw’è
	void setSymbols(const QVector<QStringList>& symbols);

	void setLineNumberFont(const QFont& font);
	const QFont& lineNumberFont() const;

	void setLineIconBackgroundColorRole(const QPalette::ColorRole& role);
	const QPalette::ColorRole& lineIconBackgroundColorRole() const;

	void setLineNumberBackgroundColorRole(const QPalette::ColorRole& role);
	const QPalette::ColorRole& lineNumberBackgroundColorRole() const;

	void setLineNumberTextColorRole(const QPalette::ColorRole& role);
	const QPalette::ColorRole& lineNumberTextColorRole() const;

	void setLineIconBackgroundColor(const QColor& color);
	const QColor& lineIconBackgroundColor() const;

	void setLineNumberBackgroundColor(const QColor& color);
	const QColor& lineNumberBackgroundColor() const;

	void setLineNumberTextColor(const QColor& color);
	const QColor& lineNumberTextColor() const;

	void setLineNumberVisible(bool visible);
	bool isLineNumberVisible() const;

	void setLineIcon(int lineNo, const QIcon& icon);
	const QIcon& lineIcon(int lineNo);
	void clearLineIcon();
	void clearLineIcon(const QIcon& icon);
	void clearLineIcon(int lineNo);
	void clearLineIcon(int lineNo, const QIcon& icon);

	void setTabStopCharWidth(int width);
	int  tabStopCharWidth() const;

protected:

	virtual void resizeEvent(QResizeEvent* event);
	virtual void keyPressEvent(QKeyEvent* event);

	void paintLineNumEvent(QPaintEvent* event);
	void paintRulerEvent(QPaintEvent* event);
	void mousePressLineNumEvent(QMouseEvent* event);
	void mouseReleaseLineNumEvent(QMouseEvent* event);
	void mouseMoveLineNumEvent(QMouseEvent* event);

signals:

	void pressIconArea(int lineNo);

public slots:

	void updateLineNumberWidth(int newBlockCount);
	void updateLineNumber(const QRect& rect ,int dy);

	void del();

protected slots:

	void insertCompletion(const QString& completion);

private:

	int lineNumberWidth();

};

inline const QPalette::ColorRole& CCodeEdit::lineIconBackgroundColorRole() const {
	return m_lineIconBackgroundColorRole;
}

inline const QPalette::ColorRole& CCodeEdit::lineNumberBackgroundColorRole() const {
	return m_lineNumberBackgroundColorRole;
}

inline const QPalette::ColorRole& CCodeEdit::lineNumberTextColorRole() const {
	return m_lineNumberTextColorRole;
}

inline const QColor& CCodeEdit::lineIconBackgroundColor() const {
	return
		QPalette::NoRole == m_lineIconBackgroundColorRole
			? m_lineIconBackgroundColor
			: palette().color(m_lineIconBackgroundColorRole);
}

inline const QColor& CCodeEdit::lineNumberBackgroundColor() const {
	return
		QPalette::NoRole == m_lineNumberBackgroundColorRole
			? m_lineNumberBackgroundColor
			: palette().color(m_lineNumberBackgroundColorRole);
}

inline const QColor& CCodeEdit::lineNumberTextColor() const {
	return
		QPalette::NoRole == m_lineNumberTextColorRole
			? m_lineNumberTextColor
			: palette().color(m_lineNumberTextColorRole);
}

inline bool CCodeEdit::isLineNumberVisible() const {
	return m_visibleLineNumber;
}

inline int CCodeEdit::tabStopCharWidth() const {
	return m_tabStopCharWidth;
}

#endif // !defined(INCLUDE_GUARD_3ED30DA5_1A87_433A_9ED9_FCEC987B4EAA)
