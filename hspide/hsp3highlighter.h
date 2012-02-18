#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>
#include "hsp3lexer.h"

//////////////////////////////////////////////////////////////////////

class CHsp3Highlighter
	: public QSyntaxHighlighter
{
private:

	QHash<QString, Hsp3Lexer::Type> m_keywords;

	QTextCharFormat m_format[Hsp3Lexer::TypeNum];

public:

	CHsp3Highlighter(QTextDocument *parent);

	virtual ~CHsp3Highlighter();

	// シンボル一覧を指定
	void setSymbols(const QVector<QStringList> &symbols);

	// 書式を指定
	void setFormat(Hsp3Lexer::Type type, const QTextCharFormat& format);

private:

	virtual void highlightBlock(const QString& text);

};

