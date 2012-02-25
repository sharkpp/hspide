#include <QLineEdit>
#include <QKeySequence>
#include <QKeyEvent>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CHotkeyWidget
	: public QLineEdit
{
	Q_OBJECT
    Q_PROPERTY(QKeySequence keySequence READ keySequence WRITE setKeySequence USER true)

	QKeySequence m_keySequence;

public:
	CHotkeyWidget(QWidget *parent = 0);
	virtual ~CHotkeyWidget();

	const QKeySequence& keySequence() const;

	void setKeySequence(const QKeySequence& keySequence);

protected:

	virtual void keyPressEvent(QKeyEvent* event);
	virtual void showEvent(QShowEvent* event);

public slots:

	void onTextChanged(const QString & text);

private:

};

inline
const QKeySequence& CHotkeyWidget::keySequence() const
{
	return m_keySequence;
}

inline
void CHotkeyWidget::setKeySequence(const QKeySequence& keySequence)
{
	m_keySequence = keySequence;
}
