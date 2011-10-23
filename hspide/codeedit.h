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

	QWidget *            mLineNumber;
	QSyntaxHighlighter * mHighlighter;

	QPalette::ColorRole  mLineNumberBackgroundColorRole;
	QPalette::ColorRole  mLineNumberTextColorRole;

	QColor               mLineNumberBackgroundColor;
	QColor               mLineNumberTextColor;

public:

	CCodeEdit(QWidget *parent = 0);

	void setLineNumberBackgroundColorRole(const QPalette::ColorRole & role);
	const QPalette::ColorRole & lineNumberBackgroundColorRole() const;

	void setLineNumberTextColorRole(const QPalette::ColorRole & role);
	const QPalette::ColorRole & lineNumberTextColorRole() const;

	void setLineNumberBackgroundColor(const QColor & color);
	const QColor & lineNumberBackgroundColor() const;

	void setLineNumberTextColor(const QColor & color);
	const QColor & lineNumberTextColor() const;

protected:

	virtual void resizeEvent(QResizeEvent * event);

	void paintLineNumEvent(QPaintEvent * event);
	void mousePressLineNumEvent(QMouseEvent * event);

public slots:

	void updateLineNumberWidth(int newBlockCount);
	void updateLineNumber(const QRect & rect ,int dy);

private:

	int lineNumberWidth();

};

inline void CCodeEdit::setLineNumberBackgroundColorRole(const QPalette::ColorRole & role) {
	mLineNumberBackgroundColorRole = role;
}

inline const QPalette::ColorRole & CCodeEdit::lineNumberBackgroundColorRole() const {
	return mLineNumberBackgroundColorRole;
}

inline void CCodeEdit::setLineNumberTextColorRole(const QPalette::ColorRole & role) {
	mLineNumberTextColorRole = role;
}

inline const QPalette::ColorRole & CCodeEdit::lineNumberTextColorRole() const {
	return mLineNumberTextColorRole;
}

inline void CCodeEdit::setLineNumberBackgroundColor(const QColor & color) {
	mLineNumberBackgroundColor = color;
	mLineNumberBackgroundColorRole = QPalette::NoRole;
}

inline const QColor & CCodeEdit::lineNumberBackgroundColor() const {
	return
		QPalette::NoRole == mLineNumberBackgroundColorRole
			? mLineNumberBackgroundColor
			: palette().color(mLineNumberBackgroundColorRole);
}

inline void CCodeEdit::setLineNumberTextColor(const QColor & color) {
	mLineNumberTextColor = color;
	mLineNumberTextColorRole = QPalette::NoRole;
}

inline const QColor & CCodeEdit::lineNumberTextColor() const {
	return
		QPalette::NoRole == mLineNumberTextColorRole
			? mLineNumberTextColor
			: palette().color(mLineNumberTextColorRole);
}

#endif // !defined(INCLUDE_GUARD_3ED30DA5_1A87_433A_9ED9_FCEC987B4EAA)
