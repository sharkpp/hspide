#include "hotkeywidget.h"
#include <QInputContext>

// とりあえずなんか動いてるっぽい...
// なぜだろう？
class CDummyInputContext : public QInputContext
{
public:
	CDummyInputContext(QObject* parent) : QInputContext(parent) {}
	virtual QString identifierName()    { return ""; }
	virtual QString language()          { return ""; }
	virtual void    reset()             { }
	virtual bool    isComposing() const { return false; }
};

CHotkeyWidget::CHotkeyWidget(QWidget *parent)
	: QLineEdit(parent)
{
//	setReadOnly(true);
	setInputContext(new CDummyInputContext(this));
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));
}

CHotkeyWidget::~CHotkeyWidget()
{
}

void CHotkeyWidget::keyPressEvent(QKeyEvent* event)
{
	m_keySequence = QKeySequence();

	int key = event->key();

	// 修飾キーの場合表示されないようにクリア
	if( Qt::Key_Shift   == key ||
		Qt::Key_Control == key ||
		Qt::Key_Meta    == key ||
		Qt::Key_Alt     == key ) // なんか適当くさいorz
	{
		key = 0;
	}

	m_keySequence
		= QKeySequence(
				(0 != (event->modifiers() & Qt::MetaModifier)    ? Qt::META  : 0) +
				(0 != (event->modifiers() & Qt::ShiftModifier)   ? Qt::SHIFT : 0) +
				(0 != (event->modifiers() & Qt::ControlModifier) ? Qt::CTRL  : 0) +
				(0 != (event->modifiers() & Qt::AltModifier)     ? Qt::ALT   : 0) +
				key
			);

	event->ignore();

	setText(m_keySequence.toString());
}

void CHotkeyWidget::showEvent(QShowEvent* event)
{
	setText(m_keySequence.toString());

	QLineEdit::showEvent(event);
}

void CHotkeyWidget::onTextChanged(const QString & text)
{
	if( text != m_keySequence.toString() ) {
		setText(m_keySequence.toString());
	}
}
