#include <QObject>
#include <QVector>
#include <QString>
#include <QColor>
#include <QAction>
#include <QKeySequence>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class Configuration
	: public QObject
{
public:

	typedef enum {
		TextMetrics = 0,		// ����
		LineNumberMetrics,		// �s�ԍ�
		FunctionMetrics,		// �֐�
		SubroutineMetrics,		// ����
		PreprocessorMetrics,	// �v���v���Z�b�T
		MacroMetrics,			// �}�N��
		LabelMetrics,			// ���x��
		CommentMetrics,			// �R�����g
		StringMetrics,			// ������
		LineFeedCharMetrics,	// ���s����
		TabCharMetrics,			// TAB����
		EofMetrics,				// EOF
		ColorMetricsNum,
	} ColorMetricsEnum;

	typedef enum {
		ShortcutNew,			// �V�K�쐬
		ShortcutOpen,			// �J��
		ShortcutSave,			// �ۑ�
		ShortcutSaveAs,			// ���O�����ĕۑ�
		ShortcutSaveAll,		// �S�ĕۑ�
		ShortcutQuit,			// �I��
		ShortcutUndo,			// ���ɖ߂�
		ShortcutRedo,			// ��蒼��
		ShortcutCut,			// �؂���
		ShortcutCopy,			// �R�s�[
		ShortcutPaste,			// �y�[�X�g
		ShortcutClear,			// �폜
		ShortcutSelectAll,		// �S�đI��
		ShortcutFind,			// ����
		ShortcutFindNext,		// ��������
		ShortcutFindPrev,		// �O������
		ShortcutReplace,		// �u��
		ShortcutJump,			// �w��s�ֈړ�
		ShortcutBuildSolution,	// �\�����[�V�������r���h
		ShortcutBuildProject,	// �v���W�F�N�g���r���h
		ShortcutCompileOnly,	// �R���p�C��
		ShortcutDebugRun,		// �f�o�b�O�J�n
		ShortcutNoDebugRun,		// �f�o�b�O�Ȃ��ŊJ�n
		ShortcutDebugSuspend,	// �f�o�b�N���f
		ShortcutDebugResume,	// �f�o�b�O�ĊJ
		ShortcutDebugStop,		// �f�o�b�O���~
		ShortcutConfig,			// �ݒ�
		ShortcutShowProject,	// �v���W�F�N�g��\��
		ShortcutShowSymbol,		// �V���{���ꗗ��\��
		ShortcutShowOutput,		// �o�͂�\��
		ShortcutShowSearch,		// �������ʂ�\��
		ShortcutShowMessage,	// ���b�Z�[�W��\��
		ShortcutShowBreakPoint,	// �u���[�N�|�C���g��\��
		ShortcutShowSysInfo,	// �V�X�e������\��
		ShortcutShowVarInfo,	// �ϐ�����\��
		ShortcutNum,
	} ShortcutEnum;

	typedef struct {
		bool   enable;
		bool   useBoldFont;
		QColor backgroundColor;
		QColor foregroundColor;
	} ColorMetricsInfoType;

	typedef struct {
		QKeySequence keys;
	} ShortcutInfoType;

	typedef struct {
		bool    enable;
		QString name;
		QString path;
		QString arguments;
		QString workDirectory;
	} ToolInfoType;

private:

	Q_OBJECT

	QString m_hspPath;
	QString m_hspCommonPath;
	QString m_compilerPath;

	int m_editorTabWidth;
	bool m_editorLineNumberVisibled;

	QString m_editorLineNumberFontName;
	int     m_editorLineNumberFontSize;
	QString m_editorFontName;
	int     m_editorFontSize;

	ColorMetricsInfoType m_colorMetrics[ColorMetricsNum];

	QVector<ToolInfoType> m_tools;

	ShortcutInfoType m_shortcutInfo[ShortcutNum];

public:

	Configuration();
	Configuration(const Configuration& info);
	Configuration& operator = (const Configuration& info);

	void setHspPath(const QString& path)
		{ m_hspPath = path; }
	const QString& hspPath() const
		{ return m_hspPath; }

	void setHspCommonPath(const QString& path)
		{ m_hspCommonPath = path; }
	const QString& hspCommonPath() const
		{ return m_hspCommonPath; }

	void setCompilerPath(const QString& path)
		{ m_compilerPath = path; }
	const QString& compilerPath() const
		{ return m_compilerPath; }


	void setEditorTabWidth(int width)
		{ m_editorTabWidth = width; }
	int editorTabWidth() const
		{ return m_editorTabWidth; }

	void setEditorLineNumberVisible(bool visibled)
		{ m_editorLineNumberVisibled = visibled; }
	bool editorLineNumberVisibled() const
		{ return m_editorLineNumberVisibled; }


	void setEditorLineNumberFontName(const QString& fontName)
		{ m_editorLineNumberFontName = fontName; }
	const QString& editorLineNumberFontName() const
		{ return m_editorLineNumberFontName; }

	void setEditorLineNumberFontSize(int size)
		{ m_editorLineNumberFontSize = size; }
	int editorLineNumberFontSize() const
		{ return m_editorLineNumberFontSize; }

	void setEditorFontName(const QString& fontName)
		{ m_editorFontName = fontName; }
	const QString& editorFontName() const
		{ return m_editorFontName; }

	void setEditorFontSize(int size)
		{ m_editorFontSize = size; }
	int editorFontSize() const
		{ return m_editorFontSize; }


	void setColorMetrics(ColorMetricsEnum type, const ColorMetricsInfoType& info)
		{ m_colorMetrics[type] = info; }
	const ColorMetricsInfoType& colorMetrics(ColorMetricsEnum type) const
		{ return m_colorMetrics[type]; }


	void setShortcut(ShortcutEnum type, const ShortcutInfoType& info)
		{ m_shortcutInfo[type] = info; }
	const ShortcutInfoType& shortcut(ShortcutEnum type) const
		{ return m_shortcutInfo[type]; }
	void applyShortcut(ShortcutEnum type, QAction* action) const
		{ action->setShortcut(m_shortcutInfo[type].keys); }


	void setToolInfo(const QVector<ToolInfoType>& info)
		{ m_tools = info; }
	const QVector<ToolInfoType>& toolInfo() const
		{ return m_tools; }

	void setToolInfo(int index, const ToolInfoType& info)
		{ m_tools[index] = info; }
	const ToolInfoType& toolInfo(int index) const
		{ return m_tools[index]; }
	int toolCount() const
		{ return m_tools.size(); }

signals:

	void updateConfiguration(const Configuration& info);

};
