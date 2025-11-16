#include "Token.hpp"

Token::Token(TokenType t, std::string_view lex, int l)
    : type{t},
      lexeme{lex},
      line{l}
{
}
