#include "outputdock.h"
#include <QtGui>

COutputDock::COutputDock(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addWidget(m_toolBar = new QToolBar(this));
	layout->addWidget(m_buildLog = new QTextEdit(this));
	layout->addWidget(m_debugLog = new QTextEdit(this));

	m_buildLog->setReadOnly(true);

	m_debugLog->setReadOnly(true);

	QComboBox* combobox;
	m_toolBar->setStyleSheet("QToolBar{border:none}");
	m_toolBar->setIconSize(QSize(16, 16));
	m_toolBar->addWidget(combobox = new QComboBox(this));
	QAction* visibledErrorAction      = m_toolBar->addAction(QIcon(":/images/tango/small/dialog-error.png"), "clear");
	QAction* visibledErrorActio2      = m_toolBar->addAction(QIcon(":/images/tango/small/dialog-error.png"), "wordwrap");

	combobox->addItem(tr("Build results"));
	combobox->addItem(tr("Debug log"));
	connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(onLogTypeChanged(int)));

	onLogTypeChanged(BuildOutput);
}

void COutputDock::onLogTypeChanged(int index)
{
	switch(index) {
	case BuildOutput:
		m_debugLog->setVisible(false);
		m_buildLog->setVisible(true);
		break;
	case DebugOutput:
		m_buildLog->setVisible(false);
		m_debugLog->setVisible(true);
		break;
	}
}

// 出力をクリア
void COutputDock::clear(OutputType type)
{
	switch(type) {
	case BuildOutput:
		m_buildLog->clear();
		break;
	case DebugOutput:
		m_debugLog->clear();
		break;
	default:
		m_buildLog->clear();
		m_debugLog->clear();
	}
}

// 出力に文字列を追記
void COutputDock::output(OutputType type, const QString & text)
{
	QTextCursor cursor;

	switch(type) {
	case BuildOutput:
		cursor = QTextCursor(m_buildLog->document());
		break;
	case DebugOutput:
		cursor = QTextCursor(m_debugLog->document());
		break;
	default:
		return;
	}

	cursor.movePosition(QTextCursor::End);
	cursor.insertText(text);
}

// 出力に文字列を追記(改行付き)
void COutputDock::outputCrLf(OutputType type, const QString & text)
{
	output(type, text + "\n");
}

