#include <QTextEdit>
#include <QToolBar>
#include <QComboBox>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class COutputDock
	: public QWidget
{
public:

	typedef enum {
		BuildOutput = 0,
		DebugOutput,
		AllOutput,
	} OutputType;

private:

	Q_OBJECT

	QToolBar*  m_toolBar;
	QTextEdit* m_buildLog;
	QTextEdit* m_debugLog;
	QComboBox* m_typeSelect;

public:

	COutputDock(QWidget *parent = 0);

	// �o�͓��e��؂�ւ�
	void select(OutputType type);

	// �o�͂��N���A
	void clear(OutputType type = AllOutput);

	// �o�͂ɕ������ǋL
	void output(OutputType type, const QString& text);
	// �o�͂ɕ������ǋL(���s�t��)
	void outputCrLf(OutputType type, const QString& text);

protected:

public slots:

	void onClearLog();
	void onLogTypeChanged(int index);

private:

};
