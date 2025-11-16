#ifndef TOKEN_HPP
#define TOKEN_HPP
#include <string>

enum class TokenType
{
    // Literals
    IDENTIFIER, NUMBER,

    // Keywords
    VOID, FLOAT, INT, BOOL,
    VEC2, VEC3, VEC4,
    IVEC2, IVEC3, IVEC4,
    BVEC2, BVEC3, BVEC4,
    MAT2, MAT3, MAT4,
    SAMPLER2D, SAMPLER_CUBE,

    // Qualifiers
    IN, OUT, INOUT, UNIFORM, ATTRIBUTE, VARYING,
    CONST, HIGHP, MEDIUMP, LOWP,

    // Control flow
    IF, ELSE, FOR, WHILE, DO, BREAK, CONTINUE, RETURN,
    DISCARD, STRUCT,

    // Operators
    PLUS, MINUS, STAR, SLASH, PERCENT,
    EQUAL, PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL,
    EQUAL_EQUAL, BANG_EQUAL,
    LESS, LESS_EQUAL, GREATER, GREATER_EQUAL,
    AMPERSAND_AMPERSAND, PIPE_PIPE,
    BANG, TILDE, AMPERSAND, PIPE, CARET,
    LESS_LESS, GREATER_GREATER,
    PLUS_PLUS, MINUS_MINUS,
    QUESTION, COLON,

    // Delimiters
    SEMICOLON, COMMA, DOT,
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,

    // Special
    PREPROCESSOR,  // #version, #define..
    WHITESPACE,
    NEWLINE,

    END_OF_FILE, ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType t, std::string_view lex, int l);
};

#endif //TOKEN_HPP
