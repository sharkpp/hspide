#include <QtGui>
#include <QDebug>
#include "hsp3highlighter.h"

CHsp3Highlighter::CHsp3Highlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	// ラベル
	QTextCharFormat& labelFormat = m_format[Hsp3Lexer::TypeLabel];
	labelFormat.setForeground(Qt::darkYellow);

	// コメント
	QTextCharFormat& commentFormat = m_format[Hsp3Lexer::TypeComment];
	commentFormat.setForeground(Qt::darkGreen);

	// 文字列
	QTextCharFormat& stringFormat = m_format[Hsp3Lexer::TypeString];
	stringFormat.setForeground(Qt::darkRed);

	// 命令
	QTextCharFormat& subroutineFormat = m_format[Hsp3Lexer::TypeSubroutine];
	subroutineFormat.setForeground(Qt::blue);

	// 関数
	QTextCharFormat& functionFormat = m_format[Hsp3Lexer::TypeFunction];
	functionFormat.setForeground(Qt::blue);

	// プリプロセッサ
	QTextCharFormat& preprocesserFormat = m_format[Hsp3Lexer::TypePreprocesser];
	preprocesserFormat.setForeground(Qt::blue);

	// 定義済みマクロ
	QTextCharFormat& macroFormat = m_format[Hsp3Lexer::TypeMacro];
	macroFormat.setForeground(QBrush(QColor(0,128,255)));
}

CHsp3Highlighter::~CHsp3Highlighter()
{
}

// シンボル一覧を指定
void CHsp3Highlighter::setSymbols(const QVector<QStringList> &symbols)
{
	m_keywords.clear();

	foreach(const QStringList &symbol, symbols)
	{
		const QString &category1 = symbol[1];
		const QString &category2 = symbol[2];
		if( !category2.compare("macro") ) {
			// 定義済みマクロ
			m_keywords[symbol[0]] = Hsp3Lexer::TypeMacro;
		} else if( !category2.compare("func") ) {
			if( !category1.compare("pre") ) {
				// プリプロセッサ
				m_keywords[symbol[0]] = Hsp3Lexer::TypePreprocesser;
			} else {
				// 関数/命令
				m_keywords[symbol[0]] = Hsp3Lexer::TypeFunction;
			}
		}
	}
}

// 書式を指定
void CHsp3Highlighter::setFormat(Hsp3Lexer::Type type, const QTextCharFormat& format)
{
	m_format[type] = format;
}

void CHsp3Highlighter::highlightBlock(const QString& text)
{
	Hsp3Lexer lexer;

	lexer.reset(QString(text).append(QChar::LineSeparator), previousBlockState());

	while( lexer.scan() )
	{
		if( Hsp3Lexer::TypeNormal == lexer.type() )
		{
			QString token = lexer.text().toLower();
			if( m_keywords.contains(token) )
			{
				QSyntaxHighlighter::setFormat(lexer.start(), lexer.length(), m_format[m_keywords[token]]);
			}
		}
		else
		{
			QSyntaxHighlighter::setFormat(lexer.start(), lexer.length(), m_format[lexer.type()]);
		}
	}

	setCurrentBlockState( lexer.state() );

}
