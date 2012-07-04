#include <QObject>
#include <QVector>
#include <QString>
#include <QColor>
#include <QAction>
#include <QKeySequence>
#include <QSettings>

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
		ActionNew,					// �V�K�쐬
		ActionOpen,					// �J��
		ActionSave,					// �ۑ�
		ActionSaveAs,				// ���O�����ĕۑ�
		ActionSaveAll,				// �S�ĕۑ�
		ActionQuit,					// �I��
		ActionUndo,					// ���ɖ߂�
		ActionRedo,					// ��蒼��
		ActionCut,					// �؂���
		ActionCopy,					// �R�s�[
		ActionPaste,				// �y�[�X�g
		ActionClear,				// �폜
		ActionSelectAll,			// �S�đI��
		ActionFind,					// ����
		ActionFindNext,				// ��������
		ActionFindPrev,				// �O������
		ActionReplace,				// �u��
		ActionJump,					// �w��s�ֈړ�
		ActionBuildSolution,		// �\�����[�V�������r���h
		ActionBuildProject,			// �v���W�F�N�g���r���h
		ActionCompileOnly,			// �R���p�C��
		ActionBatchBuild,			// �o�b�`�r���h
		ActionDebugRunSolution,		// �\�����[�V�������f�o�b�O�J�n
		ActionNoDebugRunSolution,	// �\�����[�V�������f�o�b�O�Ȃ��ŊJ�n
		ActionDebugRunProject,		// �v���W�F�N�g���f�o�b�O�J�n
		ActionNoDebugRunProject,	// �v���W�F�N�g���f�o�b�O�Ȃ��ŊJ�n
		ActionDebugSuspend,			// �f�o�b�N���f
		ActionDebugResume,			// �f�o�b�O�ĊJ
		ActionDebugStop,			// �f�o�b�O���~
		ActionDebugStepIn,			// �X�e�b�vIn
		ActionDebugStepOver,		// �X�e�b�vOver
		ActionDebugStepOut,			// �X�e�b�vOut
		ActionBreakpointSet,		// �u���[�N�|�C���g�̐ݒ�/����
		ActionConfig,				// �ݒ�
		ActionShowProject,			// �v���W�F�N�g��\��
		ActionShowSymbol,			// �V���{���ꗗ��\��
		ActionShowOutput,			// �o�͂�\��
		ActionShowSearch,			// �������ʂ�\��
		ActionShowMessage,			// ���b�Z�[�W��\��
		ActionShowBreakPoint,		// �u���[�N�|�C���g��\��
		ActionShowSysInfo,			// �V�X�e������\��
		ActionShowVarInfo,			// �ϐ�����\��
		ActionAbout,				// �o�[�W�������
		ActionNum,
		ActionSeparator = ActionNum,
	} ActionEnum;

	typedef struct {
		bool   enable;
		bool   useBoldFont;
		QColor backgroundColor;
		QColor foregroundColor;
	} ColorMetricsInfoType;

	typedef struct {
		QKeySequence keys;
	} KeyAssignInfoType;

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

	typedef struct {
		QString name;
		QVector<KeyAssignInfoType> keyAssign;
	} KeyAssignPresetInfoType;

	QVector<KeyAssignPresetInfoType> m_keyAssignInfo;

	friend bool operator == (const Configuration::KeyAssignPresetInfoType& lhs, const Configuration::KeyAssignPresetInfoType& rhs);

	typedef struct {
		QString name;
		QVector<ActionEnum> toolbar;
	} ToolbarPresetInfoType;

	QVector<ToolbarPresetInfoType> m_toolbarInfo;

	friend bool operator == (const Configuration::ToolbarPresetInfoType& lhs, const Configuration::ToolbarPresetInfoType& rhs);

public:

	Configuration();
	Configuration(const Configuration& info);
	Configuration& operator = (const Configuration& info);


	bool load(const QSettings& settings);
	bool save(QSettings& settings) const;


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


	int keyAssignPresetNum() const
		{ return m_keyAssignInfo.size() - 1; }
	const QString& keyAssignPresetName(int index) const
		{ return m_keyAssignInfo[index+1].name; }
	int currentKeyAssignPreset() const;
	void appendKeyAssignPreset(const QString& name, const QVector<KeyAssignInfoType>& info);
	void removeKeyAssignPreset(int index);

	void setKeyAssign(int index, ActionEnum type, const KeyAssignInfoType& info)
		{ m_keyAssignInfo[index+1].keyAssign[type] = info; }
	void setKeyAssign(int index, const QVector<KeyAssignInfoType>& info)
		{ m_keyAssignInfo[index+1].keyAssign = info; }
	const QVector<KeyAssignInfoType>& keyAssign(int index) const
		{ return m_keyAssignInfo[index+1].keyAssign; }
	const KeyAssignInfoType& keyAssign(int index, ActionEnum type) const
		{ return m_keyAssignInfo[index+1].keyAssign[type]; }
	void setKeyAssign(ActionEnum type, const KeyAssignInfoType& info)
		{ setKeyAssign(-1, type, info); }
	void setKeyAssign(const QVector<KeyAssignInfoType>& info)
		{ setKeyAssign(-1, info); }
	const QVector<KeyAssignInfoType>& keyAssign() const
		{ return keyAssign(-1); }
	const KeyAssignInfoType& keyAssign(ActionEnum type) const
		{ return keyAssign(-1, type); }
	void applyShortcut(ActionEnum type, QAction* action) const
		{ action->setShortcut(m_keyAssignInfo[0].keyAssign[type].keys); }


	int toolbarPresetNum() const
		{ return m_toolbarInfo.size() - 1; }
	const QString& toolbarPresetName(int index) const
		{ return m_toolbarInfo[index+1].name; }
	int currentToolbarPreset() const;
	void appendToolbarPreset(const QString& name, const QVector<ActionEnum>& info);
	void removeToolbarPreset(int index);

	void setToolbar(int index, const QVector<ActionEnum>& info)
		{ m_toolbarInfo[index+1].toolbar = info; }
	const QVector<ActionEnum>& toolbar(int index) const
		{ return m_toolbarInfo[index+1].toolbar; }
	void setToolbar(const QVector<ActionEnum>& info)
		{ setToolbar(-1, info); }
	const QVector<ActionEnum>& toolbar() const
		{ return toolbar(-1); }


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

private:

	static QStringList colorMetricsItemNames();
	static QStringList actionItemNames();

signals:

	void updateConfiguration(const Configuration& info);

};

bool operator == (const Configuration::KeyAssignInfoType& lhs, const Configuration::KeyAssignInfoType& rhs);
