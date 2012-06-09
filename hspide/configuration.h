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
		TextMetrics = 0,		// 文字
		LineNumberMetrics,		// 行番号
		FunctionMetrics,		// 関数
		SubroutineMetrics,		// 命令
		PreprocessorMetrics,	// プリプロセッサ
		MacroMetrics,			// マクロ
		LabelMetrics,			// ラベル
		CommentMetrics,			// コメント
		StringMetrics,			// 文字列
		LineFeedCharMetrics,	// 改行文字
		TabCharMetrics,			// TAB文字
		EofMetrics,				// EOF
		ColorMetricsNum,
	} ColorMetricsEnum;

	typedef enum {
		ShortcutNew,			// 新規作成
		ShortcutOpen,			// 開く
		ShortcutSave,			// 保存
		ShortcutSaveAs,			// 名前をつけて保存
		ShortcutSaveAll,		// 全て保存
		ShortcutQuit,			// 終了
		ShortcutUndo,			// 元に戻す
		ShortcutRedo,			// やり直す
		ShortcutCut,			// 切り取り
		ShortcutCopy,			// コピー
		ShortcutPaste,			// ペースト
		ShortcutClear,			// 削除
		ShortcutSelectAll,		// 全て選択
		ShortcutFind,			// 検索
		ShortcutFindNext,		// 次を検索
		ShortcutFindPrev,		// 前を検索
		ShortcutReplace,		// 置換
		ShortcutJump,			// 指定行へ移動
		ShortcutBuildSolution,	// ソリューションをビルド
		ShortcutBuildProject,	// プロジェクトをビルド
		ShortcutCompileOnly,	// コンパイル
		ShortcutDebugRun,		// デバッグ開始
		ShortcutNoDebugRun,		// デバッグなしで開始
		ShortcutDebugSuspend,	// デバック中断
		ShortcutDebugResume,	// デバッグ再開
		ShortcutDebugStop,		// デバッグ中止
		ShortcutConfig,			// 設定
		ShortcutShowProject,	// プロジェクトを表示
		ShortcutShowSymbol,		// シンボル一覧を表示
		ShortcutShowOutput,		// 出力を表示
		ShortcutShowSearch,		// 検索結果を表示
		ShortcutShowMessage,	// メッセージを表示
		ShortcutShowBreakPoint,	// ブレークポイントを表示
		ShortcutShowSysInfo,	// システム情報を表示
		ShortcutShowVarInfo,	// 変数情報を表示
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
