#include "Minifier.hpp"

#include <iostream>

void Minifier::initBuiltins()
{
    //builtin variables
    m_Builtins.insert("gl_Position");
    m_Builtins.insert("gl_FragColor");
    m_Builtins.insert("gl_FragCoord");
    m_Builtins.insert("gl_FragDepth");
    m_Builtins.insert("gl_PointSize");
    m_Builtins.insert("gl_VertexID");
    m_Builtins.insert("gl_InstanceID");
    m_Builtins.insert("gl_FrontFacing");

    // builtin functions
    m_Builtins.insert("texture");
    m_Builtins.insert("texture2D");
    m_Builtins.insert("textureCube");
    m_Builtins.insert("sin");
    m_Builtins.insert("cos");
    m_Builtins.insert("tan");
    m_Builtins.insert("asin");
    m_Builtins.insert("acos");
    m_Builtins.insert("atan");
    m_Builtins.insert("pow");
    m_Builtins.insert("exp");
    m_Builtins.insert("exp2");
    m_Builtins.insert("log");
    m_Builtins.insert("log2");
    m_Builtins.insert("sqrt");
    m_Builtins.insert("inversesqrt");
    m_Builtins.insert("abs");
    m_Builtins.insert("sign");
    m_Builtins.insert("floor");
    m_Builtins.insert("ceil");
    m_Builtins.insert("fract");
    m_Builtins.insert("mod");
    m_Builtins.insert("min");
    m_Builtins.insert("max");
    m_Builtins.insert("clamp");
    m_Builtins.insert("mix");
    m_Builtins.insert("step");
    m_Builtins.insert("smoothstep");
    m_Builtins.insert("length");
    m_Builtins.insert("distance");
    m_Builtins.insert("dot");
    m_Builtins.insert("cross");
    m_Builtins.insert("normalize");
    m_Builtins.insert("reflect");
    m_Builtins.insert("refract");
    m_Builtins.insert("faceforward");
    m_Builtins.insert("matrixCompMult");
    m_Builtins.insert("lessThan");
    m_Builtins.insert("lessThanEqual");
    m_Builtins.insert("greaterThan");
    m_Builtins.insert("greaterThanEqual");
    m_Builtins.insert("equal");
    m_Builtins.insert("notEqual");
    m_Builtins.insert("any");
    m_Builtins.insert("all");
    m_Builtins.insert("not");
    m_Builtins.insert("dFdx");
    m_Builtins.insert("dFdy");
    m_Builtins.insert("fwidth");
}

bool Minifier::isBuiltin(const std::string& name) const
{
    if (name.length() >= 3 && name[0] == 'g' && name[1] == 'l' && name[2] == '_')
        return true;

    if (name.find("__") != std::string::npos)
        return true;

    return m_Builtins.find(name) != m_Builtins.end();
}

bool Minifier::isTypeQualifier(TokenType type) const
{
    return type == TokenType::IN || type == TokenType::OUT ||
        type == TokenType::UNIFORM || type == TokenType::ATTRIBUTE ||
        type == TokenType::VARYING || type == TokenType::CONST ||
        type == TokenType::HIGHP || type == TokenType::MEDIUMP ||
        type == TokenType::LOWP || type == TokenType::INOUT;
}

bool Minifier::isType(TokenType type) const
{
    return type == TokenType::VOID || type == TokenType::FLOAT ||
        type == TokenType::INT || type == TokenType::BOOL ||
        type == TokenType::VEC2 || type == TokenType::VEC3 ||
        type == TokenType::VEC4 || type == TokenType::IVEC2 ||
        type == TokenType::IVEC3 || type == TokenType::IVEC4 ||
        type == TokenType::BVEC2 || type == TokenType::BVEC3 ||
        type == TokenType::BVEC4 || type == TokenType::MAT2 ||
        type == TokenType::MAT3 || type == TokenType::MAT4 ||
        type == TokenType::SAMPLER2D || type == TokenType::SAMPLER_CUBE;
}

std::string Minifier::getNextVarName()
{
    int num = m_VarCounter++;
    std::string result;
    num++;

    while (num > 0)
    {
        num--;
        result = char('a' + (num % 26)) + result;
        num /= 26;
    }

    return result;
}

void Minifier::collectIdentifiers()
{
    bool afterQualifierOrType{false};
    bool afterUniform{false};

    for (std::size_t i{0}; i < m_Tokens.size(); ++i)
    {
        const Token& token{m_Tokens[i]};
        if (isTypeQualifier(token.type))
        {
            afterQualifierOrType = true;
            afterUniform = (token.type == TokenType::UNIFORM);
            continue;
        }

        if (isType(token.type))
        {
            afterQualifierOrType = true;
            continue;
        }

        // identifier after a type/qualifier
        if (token.type == TokenType::IDENTIFIER && afterQualifierOrType)
        {
            const std::string& name{token.lexeme};
            // do not rename builtins or uniforms or main

            bool mustPreserve{
                isBuiltin(name) ||
                m_ProtectedNames.find(name) != m_ProtectedNames.end()
            };

            if (!mustPreserve)
            {
                if (m_Renamings.find(name) == m_Renamings.end())
                    m_Renamings[name] = getNextVarName();
            }
            afterQualifierOrType = false;
            afterUniform = false;
        }
        if (token.type == TokenType::SEMICOLON ||
            token.type == TokenType::LEFT_BRACE ||
            token.type == TokenType::RIGHT_BRACE)
        {
            afterQualifierOrType = false;
            afterUniform = false;
        }
    }
    m_Stats.setVariablesRenamed(m_Renamings.size());
}

void Minifier::collectProtectedIdentifiers()
{
    m_ProtectedNames.insert("main");

    bool afterUniform{false};
    for (std::size_t i{0}; i < m_Tokens.size(); ++i)
    {
        const Token& token{m_Tokens[i]};

        if (token.type == TokenType::UNIFORM)
        {
            afterUniform = true;
            continue;
        }

        if (afterUniform && isType(token.type))
            continue;

        if (afterUniform && token.type == TokenType::IDENTIFIER)
        {
            m_ProtectedNames.insert(token.lexeme);
            std::cout << "Protecting uniform: " << token.lexeme << '\n';
            afterUniform = false;
        }

        if (token.type == TokenType::SEMICOLON)
            afterUniform = false;
    }
}

void Minifier::collectOriginalIdentifiers()
{
    for (const Token& token : m_Tokens)
        if (token.type == TokenType::IDENTIFIER)
            m_OriginalIdentifiers.insert(token.lexeme);
}

bool Minifier::needsSpaceBetween(const Token& prev, const Token& curr)
{
    if ((prev.type == TokenType::IDENTIFIER || isType(prev.type) || isTypeQualifier(prev.type))
        && (curr.type == TokenType::IDENTIFIER || isType(curr.type) || isTypeQualifier(curr.type)))
        return true;

    if (prev.type == TokenType::RETURN && curr.type != TokenType::SEMICOLON)
        return true;

    return false;
}

void Minifier::buildSymbolTable()
{
    bool afterQualifierOrType{false};
    bool afterUniform{false};
    TokenType lastQualifier{TokenType::ERROR};

    for (std::size_t i{0}; i < m_Tokens.size(); ++i)
    {
        const Token& token{m_Tokens[i]};

        if (isTypeQualifier(token.type))
        {
            afterQualifierOrType = true;
            lastQualifier = token.type;
            afterUniform = (token.type == TokenType::UNIFORM);
            continue;
        }

        if (isType(token.type))
        {
            afterQualifierOrType = true;
            continue;
        }

        // declaration
        if (token.type == TokenType::IDENTIFIER && afterQualifierOrType)
        {
            // std::cout << "DECLARING: " << token.lexeme << " at line " << token.line << "\n";

            SymbolKind kind{SymbolKind::VARIABLE};
            if (lastQualifier == TokenType::UNIFORM)
            {
                kind = SymbolKind::UNIFORM;
                m_Stats.setUniformsFound(m_Stats.getUniformsFound() + 1);
            }

            // check if it is function
            if (i + 1 < m_Tokens.size() && m_Tokens[i + 1].type == TokenType::LEFT_PAREN)
            {
                kind = SymbolKind::FUNCTION;
                m_Stats.setFunctionsFound(m_Stats.getFunctionsFound() + 1);
            }
            m_SymbolTable.declareSymbol(token.lexeme, kind, token.line);

            afterQualifierOrType = false;
            afterUniform = false;
            lastQualifier = TokenType::ERROR;
        }

        // usage found, identifier not after a type
        if (token.type == TokenType::IDENTIFIER && !afterQualifierOrType)
        {
            // dont count if swizzle
            if (i > 0 && m_Tokens[i - 1].type != TokenType::DOT)
            {
                // std::cout << "USING: " << token.lexeme << " at line " << token.line << "\n";
                m_SymbolTable.useSymbol(token.lexeme, token.line);
            }
        }

        if (token.type == TokenType::SEMICOLON ||
            token.type == TokenType::LEFT_BRACE ||
            token.type == TokenType::RIGHT_BRACE)
        {
            afterQualifierOrType = false;
            afterUniform = false;
            lastQualifier = TokenType::ERROR;
        }
    }
}

std::string Minifier::generateOutput()
{
    std::string result;
    Token prevToken{TokenType::END_OF_FILE, "", 0};

    for (const Token& token : m_Tokens)
    {
        if (token.type == TokenType::END_OF_FILE)
            break;

        if (token.type == TokenType::PREPROCESSOR)
        {
            if (!result.empty())
                result += '\n';

            result += token.lexeme;
            result += '\n';
            prevToken = token;
            continue;
        }

        if (!result.empty() && result.back() != '\n' && needsSpaceBetween(prevToken, token))
            result += ' ';

        if (token.type == TokenType::IDENTIFIER)
        {
            // dont rename swizzle components
            if (prevToken.type == TokenType::DOT)
                result += token.lexeme;
            else
            {
                auto it{m_Renamings.find(token.lexeme)};
                if (it != m_Renamings.end())
                    result += it->second;
                else
                    result += token.lexeme;
            }
        }
        else
            result += token.lexeme;

        prevToken = token;
    }

    return result;
}

void Minifier::printStats()
{
    m_Stats.print();
}

void Minifier::printDeadCode()
{
    m_SymbolTable.print();
}

Minifier::Minifier(const std::vector<Token>& tokens)
    : m_Tokens{tokens}
{
    initBuiltins();
}

std::string Minifier::minify()
{
    m_Stats.startTiming();

    collectProtectedIdentifiers();
    collectOriginalIdentifiers();

    buildSymbolTable();
    collectIdentifiers();
    std::string result{generateOutput()};

    m_Stats.stopTiming();
    m_Stats.setMinifiedSize(result.length());
    m_Stats.setDeadCodeRemoved(m_SymbolTable.getUnusedCount());

    return result;
}

void Minifier::printRenamings()
{
    std::cout << "Variable renamings:\n";
    for (const auto& pair : m_Renamings)
        std::cout << "\t" << pair.first << " -> " << pair.second << '\n';
}
