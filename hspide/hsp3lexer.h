#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef INCLUDE_GUARD_7819799D_3393_4CF7_8DA3_2046BDCD2E71
#define INCLUDE_GUARD_7819799D_3393_4CF7_8DA3_2046BDCD2E71

class Hsp3Lexer
{
public:

	typedef enum {
		TypeNormal = 0,
		TypeString, // "abc"
		TypeChar,   // '*'
		TypeNumber,
		TypeLabel,
		TypeComment,
		TypeFunction,
		TypeSubroutine,
		TypePreprocesser,
		TypeMacro,
		TypeNum,
	} Type;

private:

	bool m_inComment;
	bool m_isBlockComment;
	bool m_inString;
	bool m_inChar;
	bool m_inLabel;
	bool m_prevEscape;

	QString m_text;
	int     m_pos;
	int     m_line;
	QString m_token;
	int     m_start;
	int     m_length;
	Type    m_type;
	int     m_lineNo;

public:

	Hsp3Lexer();
	~Hsp3Lexer();

	void reset(const QString& text = QString(), int state = -1);

	bool scan();

	int state() const;

	const QString& text() const;
	int  start() const;
	int  length() const;
	Type type() const;
	int  lineNo() const;
};

#ifdef QDEBUG_H
inline
QDebug& operator<<(QDebug& debug, const Hsp3Lexer::Type& type)
{
	switch(type) {
	case Hsp3Lexer::TypeString:
		debug << "string ";
		break;
	case Hsp3Lexer::TypeChar:
		debug << "char   ";
		break;
	case Hsp3Lexer::TypeNumber:
		debug << "number ";
		break;
	case Hsp3Lexer::TypeLabel:
		debug << "label  ";
		break;
	case Hsp3Lexer::TypeComment:
		debug << "comment";
		break;
	case Hsp3Lexer::TypeNormal:
		debug << "normal ";
		break;
	default:
		debug << "?      ";
		break;
	}
	return debug;
}
#endif

inline
Hsp3Lexer::Hsp3Lexer()
{
	reset();
}

inline
Hsp3Lexer::~Hsp3Lexer()
{
}

inline
const QString& Hsp3Lexer::text() const
{
	return m_token;
}

inline
int Hsp3Lexer::lineNo() const
{
	return m_lineNo;
}

inline
int Hsp3Lexer::start() const
{
	return m_start;
}

inline
int Hsp3Lexer::length() const
{
	return m_length;
}

inline
Hsp3Lexer::Type Hsp3Lexer::type() const
{
	return m_type;
}

inline
int Hsp3Lexer::state() const
{
	return
		( m_prevEscape     ?  1 : 0) |
		( m_inComment      ?  2 : 0) |
		( m_isBlockComment ?  4 : 0) |
		( m_inString       ?  8 : 0) |
		( m_inChar         ? 16 : 0) |
		( m_inLabel        ? 32 : 0)
		;
}

inline
void Hsp3Lexer::reset(const QString& text, int state)
{
	m_text           = text;
	m_pos            = 0;
	m_line           = 1;
	m_lineNo         = -1;
	m_prevEscape     = 0 <= state ? (0 != (state &  1)) : false;
	m_inComment      = 0 <= state ? (0 != (state &  2)) : false;
	m_isBlockComment = 0 <= state ? (0 != (state &  4)) : false;
	m_inString       = 0 <= state ? (0 != (state &  8)) : false;
	m_inChar         = 0 <= state ? (0 != (state & 16)) : false;
	m_inLabel        = 0 <= state ? (0 != (state & 32)) : false;
}

inline
bool Hsp3Lexer::scan()
{
#ifdef _MSC_VER
#define NOOP __noop
#else
#define NOOP ((void)0)
#endif
#define LABEL_RESET() \
	if( inLabel ) { \
		inLabel = false; \
		READ_TOKEN(NOOP); \
		tokenStart = pos; \
	}
#define READ_TOKEN(code) \
	if( 0 <= tokenStart ) { \
		if( 0 < (pos - tokenStart) ) { \
			m_start  = tokenStart; \
			m_length = pos - tokenStart; \
/*QString token_ = QString(m_token).replace(QRegExp("\\[.*\\]"), ""); */ \
			m_token  = m_text.mid(m_start, m_length); \
			bool ok = false; \
			     if( inComment               )    { m_type = TypeComment; } \
			else if( inLabel && 1 < m_length )    { m_type = TypeLabel;   } \
			else if( inString                )    { m_type = TypeString;  } \
			else if( inChar                  )    { m_type = TypeChar;    } \
			else if( m_token.toInt(&ok, 10), ok ) { m_type = TypeNumber;  } \
			else if( m_token.toDouble(&ok), ok )  { m_type = TypeNumber;  } \
			else if( m_token.mid(2).toInt(&ok, 16), ( "0x" == m_token.left(2) && ok) ) { m_type = TypeNumber; } \
			else if( m_token.mid(3).toInt(&ok, 16), ("-0x" == m_token.left(3) && ok) ) { m_type = TypeNumber; } \
			else if( m_token.mid(1).toInt(&ok, 16), ( "$"  == m_token.left(1) && ok) ) { m_type = TypeNumber; } \
			else if( m_token.mid(2).toInt(&ok, 16), ("-$"  == m_token.left(2) && ok) ) { m_type = TypeNumber; } \
			else if( m_token.mid(1).toInt(&ok,  2), ( "%"  == m_token.left(1) && ok) ) { m_type = TypeNumber; } \
			else if( m_token.mid(2).toInt(&ok,  2), ("-%"  == m_token.left(2) && ok) ) { m_type = TypeNumber; } \
			else if( m_token.mid(2).toInt(&ok,  2), ( "0b" == m_token.left(2) && ok) ) { m_type = TypeNumber; } \
			else if( m_token.mid(3).toInt(&ok,  2), ("-0b" == m_token.left(3) && ok) ) { m_type = TypeNumber; } \
			else                                  { m_type = TypeNormal; } \
			     if( inPreProcess )               { m_token = m_token.remove(QChar(' ')).remove(QChar('\t')); } \
			code; \
/*m_token = "[" + token_ + "]" + m_token;*/ \
			m_lineNo         = lineNo; \
			m_pos            = pos; \
			m_inComment      = inComment; \
			m_isBlockComment = isBlockComment; \
			m_inString       = inString; \
			m_inChar         = inChar; \
			m_inLabel        = inLabel; \
			m_prevEscape     = prevEscape; \
			return true; \
		} \
		tokenStart = -1; \
	}

	bool inComment      = m_inComment;
	bool isBlockComment = m_isBlockComment;
	bool inString       = m_inString;
	bool inChar         = m_inChar;
	bool inLabel        = m_inLabel;
	bool inPreProcess   = false;
	bool skipPreProcessSpace = false; // プリプロセッサの場合に初回のみスペースを無視するため
	bool prevEscape     = m_prevEscape;
	int tokenStart = -1;
	int pos = m_pos;
	int lineNo = -1;
	for(int len = m_text.size();
		pos < len; )
	{
		const QChar c = m_text.at(pos);

		if( tokenStart < 0 ) {
			tokenStart = pos;
			lineNo     = m_line;
		}

		switch( c.unicode() )
		{
		case '+':
		case '-':
		case '*':
		case '\\': // escape or mod
		case '/':
		case '|':
		case '&':
		case '^':
		case '=':
			LABEL_RESET();
			if( inComment ) {
				if( isBlockComment &&
					pos + 1 < len &&
					'*' == c &&
					'/' == m_text.at(pos + 1) )
				{
					pos += 2;
					READ_TOKEN(inComment = false);
					pos--;
					inComment = false;
				}
				break;
			}
			if( inString || inChar ) {
				if( '\\' == c.unicode() ) {
					prevEscape = !prevEscape;
					pos++;
					continue;
				}
				break;
			}
			READ_TOKEN(NOOP);
			tokenStart = pos;
			lineNo     = m_line;
			if( len <= pos + 1 ) {
				pos--;
				READ_TOKEN(NOOP);
				pos++;
				tokenStart = pos;
				lineNo     = m_line;
				READ_TOKEN(NOOP);
				break;
			} else {
				const QChar cc = m_text.at(pos + 1);
				if( '=' == cc ) { // "+=" or "-=" or "*=" or "/=" or "|=" or "&=" or "^=" or "=="
					tokenStart = pos;
					lineNo     = m_line;
					pos += 2;
					READ_TOKEN(NOOP);
					pos--;
					break;
				} else if( c == cc ) {
					switch( c.unicode() )
					{
					case '-':
						// ↑こいつは厄介
						// a=b--1 だと a = b - (-1) になる
						// a=b-1 だと a = b - 1 になる
					case '+':
					case '|':
					case '&':
						// "++" or "--" or "||" or "&&" "||" or "=="
						tokenStart = pos;
						lineNo     = m_line;
						pos += 2;
						READ_TOKEN(NOOP);
						pos--;
						break;
					case '/':
						// "//"
						inComment = true;
						isBlockComment = false;
						break;
					default:
						pos++;
						READ_TOKEN(NOOP);
						pos--;
						break;
					}
				} else if( '/' == c && '*' == cc ) {
					inComment = true;
					isBlockComment = true;
					pos++;
				} else if( '*' == c ) {
					inLabel = true;
				} else {
					if( '-' == c && (cc.isNumber() || '$' == cc || '%' == cc || '0' == cc) ) { // -0 or -0x1 or -$1
						tokenStart = pos;
						lineNo     = m_line;
						break;
					} else {
						pos++;
						READ_TOKEN(NOOP);
						pos--;
					}
				}
			}
			break;
		case '{':
		case '}':
		case '(':
		case ')':
		case ',':
		case ':':
			LABEL_RESET();
			if( inComment || inString || inChar ) {
				break;
			}
			if( '{' == c.unicode() &&
				pos + 1 < len ) {
				const QChar cc = m_text.at(pos + 1);
				if( '\"' == cc.unicode() ) {
					inString = !inString;
					pos++;
					break;
				}
			}
			READ_TOKEN(NOOP);
			tokenStart = pos;
			lineNo     = m_line;
			pos++;
			READ_TOKEN(NOOP);
			pos--;
			break;
		case '"':
			LABEL_RESET();
			if( inComment ) {
				break;
			}
			if( !prevEscape ) {
				if( inString ) { // 文字列から復帰
					if( pos + 1 < len ) {
						const QChar cc = m_text.at(pos + 1);
						if( '}' == cc.unicode() ) {
							pos++;
						}
					}
					pos++;
					READ_TOKEN(inString = false);
					pos--;
				}
				inString = !inString;
			}
			break;
		case '\'':
			LABEL_RESET();
			if( inComment ) {
				break;
			}
			if( !prevEscape ) {
				if( inChar ) { // 文字から復帰
					pos++;
					READ_TOKEN(inChar = false);
					pos--;
				}
				inChar = !inChar;
			}
			break;
		case ';':
			LABEL_RESET();
			if( inComment || inString || inChar ) {
				break;
			}
			READ_TOKEN(NOOP);
			tokenStart = pos;
			lineNo     = m_line;
			inComment = true;
			isBlockComment = false;
			break;
		case '#':
			LABEL_RESET();
			if( inComment || inString || inChar ) {
				break;
			}
			READ_TOKEN(NOOP);
			tokenStart = pos;
			lineNo     = m_line;
			inPreProcess = true;
			skipPreProcessSpace = true;
			break;
		default:
			if( QChar::LineSeparator == c && inComment ) {
				READ_TOKEN((inComment = isBlockComment, pos++, m_line++));
				inComment = isBlockComment;
			} else if( QChar::LineSeparator == c && inString ) {
				READ_TOKEN((pos++, m_line++));
			} else if( c.isSpace() ) {
				if( !skipPreProcessSpace &&
					!inString && !inChar && !inComment )
				{
					READ_TOKEN((inLabel = false, pos++, m_line += QChar::LineSeparator == c));
					inLabel = false;
				}
			} else if( inLabel && 1 == (pos - tokenStart) && (c.isNumber() || c.isSymbol() || '.' == c.unicode()) ) {
				READ_TOKEN((inLabel = false));
				inLabel = false;
			} else {
				skipPreProcessSpace = false;
			}
			if( QChar::LineSeparator == c ) {
				m_line++;
			}
			break;
		}

		prevEscape = false;
		pos++;
	}
	READ_TOKEN(NOOP);

	return false;
}

#endif // !defined(7819799D_3393_4CF7_8DA3_2046BDCD2E71)
