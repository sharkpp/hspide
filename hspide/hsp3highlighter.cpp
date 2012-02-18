#include <QtGui>
#include <QDebug>
#include "hsp3highlighter.h"

CHsp3Highlighter::CHsp3Highlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	// ���x��
	QTextCharFormat& labelFormat = m_format[Hsp3Lexer::TypeLabel];
	labelFormat.setForeground(Qt::darkYellow);

	// �R�����g
	QTextCharFormat& commentFormat = m_format[Hsp3Lexer::TypeComment];
	commentFormat.setForeground(Qt::darkGreen);

	// ������
	QTextCharFormat& stringFormat = m_format[Hsp3Lexer::TypeString];
	stringFormat.setForeground(Qt::darkRed);

	// ����
	QTextCharFormat& subroutineFormat = m_format[Hsp3Lexer::TypeSubroutine];
	subroutineFormat.setForeground(Qt::blue);

	// �֐�
	QTextCharFormat& functionFormat = m_format[Hsp3Lexer::TypeFunction];
	functionFormat.setForeground(Qt::blue);

	// �v���v���Z�b�T
	QTextCharFormat& preprocesserFormat = m_format[Hsp3Lexer::TypePreprocesser];
	preprocesserFormat.setForeground(Qt::blue);

	// ��`�ς݃}�N��
	QTextCharFormat& macroFormat = m_format[Hsp3Lexer::TypeMacro];
	macroFormat.setForeground(QBrush(QColor(0,128,255)));
}

CHsp3Highlighter::~CHsp3Highlighter()
{
}

// �V���{���ꗗ���w��
void CHsp3Highlighter::setSymbols(const QVector<QStringList> &symbols)
{
	m_keywords.clear();

	foreach(const QStringList &symbol, symbols)
	{
		const QString &category1 = symbol[1];
		const QString &category2 = symbol[2];
		if( !category2.compare("macro") ) {
			// ��`�ς݃}�N��
			m_keywords[symbol[0]] = Hsp3Lexer::TypeMacro;
		} else if( !category2.compare("func") ) {
			if( !category1.compare("pre") ) {
				// �v���v���Z�b�T
				m_keywords[symbol[0]] = Hsp3Lexer::TypePreprocesser;
			} else {
				// �֐�/����
				m_keywords[symbol[0]] = Hsp3Lexer::TypeFunction;
			}
		}
	}
}

// �������w��
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
