#ifndef SCANNER_HPP
#define SCANNER_HPP
#include <vector>
#include <string>
#include <unordered_map>

#include "ErrorReporter.hpp"
#include "Token.hpp"


class Scanner
{
private:
    std::string m_Source;
    std::vector<Token> m_Tokens;
    ErrorReporter* m_ErrorReporter;

    int m_Start{0};
    int m_Current{0};
    int m_Line{1};
    int m_Column{1};

    std::unordered_map<std::string, TokenType> m_Keywords;


    void initKeywords();
    inline bool isAtEnd() const { return m_Current >= m_Source.length(); }

    inline char advance()
    {
        m_Column++;
        return m_Source[m_Current++];
    }

    inline char peek() const { return isAtEnd() ? '\0' : m_Source[m_Current]; }
    inline char peekNext() const { return ((m_Current + 1) >= m_Source.length()) ? '\0' : m_Source[m_Current + 1]; }
    bool match(char expected);
    void addToken(TokenType t);
    void skipWhitespace();
    void number();
    void identifier();
    void preprocessor();
    void scanToken();
    void reportError(const std::string& message);

public:
    Scanner(std::string_view src, ErrorReporter* reporter = nullptr);

    std::vector<Token> scanTokens();
};


#endif //SCANNER_HPP
