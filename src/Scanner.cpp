#include "Scanner.hpp"

#include <sstream>

void Scanner::initKeywords()
{
    // Types
    m_Keywords["void"] = TokenType::VOID;
    m_Keywords["float"] = TokenType::FLOAT;
    m_Keywords["int"] = TokenType::INT;
    m_Keywords["bool"] = TokenType::BOOL;
    m_Keywords["vec2"] = TokenType::VEC2;
    m_Keywords["vec3"] = TokenType::VEC3;
    m_Keywords["vec4"] = TokenType::VEC4;
    m_Keywords["ivec2"] = TokenType::IVEC2;
    m_Keywords["ivec3"] = TokenType::IVEC3;
    m_Keywords["ivec4"] = TokenType::IVEC4;
    m_Keywords["bvec2"] = TokenType::BVEC2;
    m_Keywords["bvec3"] = TokenType::BVEC3;
    m_Keywords["bvec4"] = TokenType::BVEC4;
    m_Keywords["mat2"] = TokenType::MAT2;
    m_Keywords["mat3"] = TokenType::MAT3;
    m_Keywords["mat4"] = TokenType::MAT4;
    m_Keywords["sampler2D"] = TokenType::SAMPLER2D;
    m_Keywords["samplerCube"] = TokenType::SAMPLER_CUBE;

    // Qualifiers
    m_Keywords["in"] = TokenType::IN;
    m_Keywords["out"] = TokenType::OUT;
    m_Keywords["inout"] = TokenType::INOUT;
    m_Keywords["uniform"] = TokenType::UNIFORM;
    m_Keywords["attribute"] = TokenType::ATTRIBUTE;
    m_Keywords["varying"] = TokenType::VARYING;
    m_Keywords["const"] = TokenType::CONST;
    m_Keywords["highp"] = TokenType::HIGHP;
    m_Keywords["mediump"] = TokenType::MEDIUMP;
    m_Keywords["lowp"] = TokenType::LOWP;

    // Control flow
    m_Keywords["if"] = TokenType::IF;
    m_Keywords["else"] = TokenType::ELSE;
    m_Keywords["for"] = TokenType::FOR;
    m_Keywords["while"] = TokenType::WHILE;
    m_Keywords["do"] = TokenType::DO;
    m_Keywords["break"] = TokenType::BREAK;
    m_Keywords["continue"] = TokenType::CONTINUE;
    m_Keywords["return"] = TokenType::RETURN;
    m_Keywords["discard"] = TokenType::DISCARD;
    m_Keywords["struct"] = TokenType::STRUCT;
}

bool Scanner::match(char expected)
{
    if (isAtEnd())
        return false;
    if (m_Source[m_Current] != expected)
        return false;
    m_Current++;
    return true;
}

void Scanner::addToken(TokenType t)
{
    std::string text{m_Source.substr(m_Start, m_Current - m_Start)};
    m_Tokens.push_back(Token{t, text, m_Line});
}

void Scanner::skipWhitespace()
{
    while (!isAtEnd())
    {
        char c{peek()};
        if (c == ' ' || c == '\r' || c == '\t')
            advance();
        else if (c == '\n')
        {
            m_Line++;
            advance();
        }
        else if (c == '/' && peekNext() == '/') // this is a comment, go to end of the line
            while (peek() != '\n' && !isAtEnd())
                advance();
        else if (c == '/' && peekNext() == '*') //multiline comment
        {
            advance();
            advance();
            while (!isAtEnd() && !(peek() == '*' && peekNext() == '/'))
            {
                if (peek() == '\n') m_Line++;
                advance();
            }
            if (!isAtEnd())
            {
                advance();
                advance();
            }
        }
        else
            break;
    }
}

void Scanner::number()
{
    while (std::isdigit(peek()))
        advance();

    // decimal
    if (peek() == '.' && std::isdigit(peekNext()))
    {
        advance();
        while (std::isdigit(peek()))
            advance();
    }

    // exponent
    if (peek() == 'e' || peek() == 'E')
    {
        advance();
        if (peek() == '+' || peek() == '-')
            advance();
        while (std::isdigit(peek()))
            advance();
    }

    // float
    if (peek() == 'f' || peek() == 'F')
        advance();

    addToken(TokenType::NUMBER);
}

void Scanner::identifier()
{
    while (std::isalnum(peek()) || peek() == '_')
        advance();

    std::string text{m_Source.substr(m_Start, m_Current - m_Start)};
    auto type{TokenType::IDENTIFIER};
    auto it{m_Keywords.find(text)};

    if (it != m_Keywords.end())
        type = it->second;

    addToken(type);
}

void Scanner::preprocessor()
{
    while (peek() != '\n' && !isAtEnd())
        advance();

    addToken(TokenType::PREPROCESSOR);
}

void Scanner::scanToken()
{
    char c{advance()};

    switch (c)
    {
    case '(':
        addToken(TokenType::LEFT_PAREN);
        break;

    case ')':
        addToken(TokenType::RIGHT_PAREN);
        break;

    case '{':
        addToken(TokenType::LEFT_BRACE);
        break;

    case '}':
        addToken(TokenType::RIGHT_BRACE);
        break;

    case '[':
        addToken(TokenType::LEFT_BRACKET);
        break;

    case ']':
        addToken(TokenType::RIGHT_BRACKET);
        break;

    case ';':
        addToken(TokenType::SEMICOLON);
        break;

    case ',':
        addToken(TokenType::COMMA);
        break;

    case '.':
        addToken(TokenType::DOT);
        break;

    case '?':
        addToken(TokenType::QUESTION);
        break;

    case ':':
        addToken(TokenType::COLON);
        break;

    case '~':
        addToken(TokenType::TILDE);
        break;

    case '+':
        if (match('+'))
            addToken(TokenType::PLUS_PLUS);
        else if (match('='))
            addToken(TokenType::PLUS_EQUAL);
        else
            addToken(TokenType::PLUS);
        break;

    case '-':
        if (match('-'))
            addToken(TokenType::MINUS_MINUS);
        else if (match('='))
            addToken(TokenType::MINUS_EQUAL);
        else
            addToken(TokenType::MINUS);
        break;

    case '*':
        addToken(match('=') ? TokenType::STAR_EQUAL : TokenType::STAR);
        break;

    case '/':
        addToken(match('=') ? TokenType::SLASH_EQUAL : TokenType::SLASH);
        break;

    case '%': addToken(TokenType::PERCENT);
        break;

    case '!':
        addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;

    case '=':
        addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;

    case '<':
        if (match('<')) addToken(TokenType::LESS_LESS);
        else if (match('=')) addToken(TokenType::LESS_EQUAL);
        else addToken(TokenType::LESS);
        break;

    case '>':
        if (match('>')) addToken(TokenType::GREATER_GREATER);
        else if (match('=')) addToken(TokenType::GREATER_EQUAL);
        else addToken(TokenType::GREATER);
        break;

    case '&':
        if (match('&')) addToken(TokenType::AMPERSAND_AMPERSAND);
        else addToken(TokenType::AMPERSAND);
        break;

    case '|':
        if (match('|')) addToken(TokenType::PIPE_PIPE);
        else addToken(TokenType::PIPE);
        break;

    case '^':
        addToken(TokenType::CARET);
        break;

    case '#':
        preprocessor();
        break;

    default:
        if (std::isdigit(c))
        {
            number();
        }
        else if (std::isalpha(c) || c == '_')
        {
            identifier();
        }
        else
        {
            std::stringstream ss;
            ss << "Unexpected character: '" << c << "'";
            reportError(ss.str());
        }
        break;
    }
}

void Scanner::reportError(const std::string& message)
{
    if (m_ErrorReporter)
    {
        int lineStart{m_Current};
        while (lineStart > 0 && m_Source[lineStart - 1] != '\n')
            lineStart--;

        int lineEnd{m_Current};
        while (lineEnd < m_Source.length() && m_Source[lineEnd] != '\n')
            lineEnd++;

        std::string context{m_Source.substr(lineStart, lineEnd - lineStart)};
        m_ErrorReporter->reportError(ErrorSeverity::ERROR, message, m_Line, m_Column, context);
    }
}

Scanner::Scanner(std::string_view src, ErrorReporter* reporter)
    : m_Source{src}, m_ErrorReporter{reporter}
{
    initKeywords();
}

std::vector<Token> Scanner::scanTokens()
{
    while (!isAtEnd())
    {
        m_Start = m_Current;
        skipWhitespace();
        if (!isAtEnd())
        {
            m_Start = m_Current;
            scanToken();
        }
    }

    m_Tokens.push_back(Token{TokenType::END_OF_FILE, "", m_Line});
    return m_Tokens;
}
