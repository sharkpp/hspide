#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class Hsp3Lexer
{
public:

	typedef enum {
		TypeNormal = 0,
		TypeString,
		TypeLabel,
		TypeComment,
		TypeNum,
	} Type;

private:

	bool m_inComment;
	bool m_isBlockComment;
	bool m_inString;
	bool m_inLabel;
	bool m_prevEscape;

	QString m_text;
	int     m_pos;
	QString m_token;
	int     m_start;
	int     m_length;
	Type    m_type;

public:

	Hsp3Lexer();
	~Hsp3Lexer();

	void reset(const QString& text = QString(), int state = -1);

	bool scan();

	int state() const;

	const QString& text() const;
	int start() const;
	int length() const;
	Type type() const;
};


QDebug& operator<<(QDebug& debug, const Hsp3Lexer::Type& type)
{
	switch(type) {
	case Hsp3Lexer::TypeString:
		debug << "string ";
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
		( m_inLabel        ? 16 : 0)
		;
}

inline
void Hsp3Lexer::reset(const QString& text, int state)
{
	m_text           = text;
	m_pos            = 0;
	m_prevEscape     = 0 <= state ? (0 != (state &  1)) : false;
	m_inComment      = 0 <= state ? (0 != (state &  2)) : false;
	m_isBlockComment = 0 <= state ? (0 != (state &  4)) : false;
	m_inString       = 0 <= state ? (0 != (state &  8)) : false;
	m_inLabel        = 0 <= state ? (0 != (state & 16)) : false;
}

inline
bool Hsp3Lexer::scan()
{
#define NOOP __noop
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
			m_token  = m_text.mid(m_start, m_length); \
			     if( inComment ) { m_type = TypeComment; } \
			else if( inLabel   ) { m_type = TypeLabel;   } \
			else if( inString  ) { m_type = TypeString;  } \
			else                 { m_type = TypeNormal;  } \
			code; \
			m_pos            = pos; \
			m_inComment      = inComment; \
			m_isBlockComment = isBlockComment; \
			m_inString       = inString; \
			m_inLabel        = inLabel; \
			m_prevEscape     = prevEscape; \
			return true; \
		} \
		tokenStart = -1; \
	}

	bool inComment      = m_inComment;
	bool isBlockComment = m_isBlockComment;
	bool inString       = m_inString;
	bool inLabel        = m_inLabel;
	bool prevEscape     = m_prevEscape;
	int tokenStart = -1;
	int pos = m_pos;
	for(int len = m_text.size();
		pos < len; )
	{
		const QChar c = m_text.at(pos);

		if( tokenStart < 0 ) {
			tokenStart = pos;
		}

		switch( c.unicode() )
		{
		case '+':
		case '-':
		case '*':
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
			if( inString ) {
				break;
			}
			READ_TOKEN(NOOP);
			tokenStart = pos;
			if( len <= pos + 1 ) {
				pos--;
				READ_TOKEN(NOOP);
				tokenStart = pos;
				pos++;
				READ_TOKEN(NOOP);
				break;
			} else {
				const QChar cc = m_text.at(pos + 1);
				if( '=' == cc ) { // "+=" or "-=" or "*=" or "/=" or "|=" or "&=" or "^=" or "=="
					tokenStart = pos;
					pos += 2;
					READ_TOKEN(NOOP);
					pos--;
					break;
				} else if( c == cc ) {
					switch( c.unicode() )
					{
					case '-':
						// ª‚±‚¢‚Â‚Í–ï‰î
						// a=b--1 ‚¾‚Æ a = b - (-1) ‚É‚È‚é
					case '+':
					case '|':
					case '&':
						// "++" or "--" or "||" or "&&" "||" or "=="
						tokenStart = pos;
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
					if( '-' == c && (cc.isNumber() || '$' == cc) ) { // -0 or -0x1 or -$1
						tokenStart = pos;
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
			if( inComment || inString ) {
				break;
			}
			READ_TOKEN(NOOP);
			tokenStart = pos;
			pos++;
			READ_TOKEN(NOOP);
			pos--;
			break;
		case '\\':
			LABEL_RESET();
			if( inComment || inString ) {
				break;
			}
			prevEscape = !prevEscape;
			pos++;
			continue;
		case '"':
			LABEL_RESET();
			if( inComment ) {
				break;
			}
			if( !prevEscape ) {
				if( inString ) { // •¶Žš—ñ‚©‚ç•œ‹A
					pos++;
					READ_TOKEN(inString = false);
					pos--;
				}
				inString = !inString;
			}
			break;
		case ';':
			LABEL_RESET();
			if( inComment || inString ) {
				break;
			}
			READ_TOKEN(NOOP);
			tokenStart = pos;
			inComment = true;
			isBlockComment = false;
			break;
		default:
			if( QChar::LineSeparator == c && inComment/* && !isBlockComment*/ ) {
				READ_TOKEN((inComment = isBlockComment, pos++));
				inComment = isBlockComment;
			} else if( c.isSpace() ) {
				if( !inString && !inComment ) {
					READ_TOKEN((inLabel = false, pos++));
					inLabel = false;
					//
				//	for(; pos < len && m_text.at(pos).isSpace();
				//		pos++);
				//	pos--;
				}
			}
			break;
		}

		prevEscape = false;
		pos++;
	}
	READ_TOKEN(NOOP);

	return false;
}
