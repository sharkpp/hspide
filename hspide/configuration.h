#include <QObject>
#include <QVector>
#include <QString>
#include <QColor>

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

	typedef struct {
		bool   enable;
		bool   useBoldFont;
		QColor backgroundColor;
		QColor foregroundColor;
	} ColorMetricsInfoType;

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
