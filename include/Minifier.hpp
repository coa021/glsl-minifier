#ifndef MINIFIER_HPP
#define MINIFIER_HPP
#include <set>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "MinificationStats.hpp"
#include "SymbolTable.hpp"
#include "Token.hpp"


class Minifier
{
private:
    std::vector<Token> m_Tokens;
    std::unordered_map<std::string, std::string> m_Renamings;
    std::unordered_set<std::string> m_Builtins;

    std::unordered_set<std::string> m_ProtectedNames;
    std::unordered_set<std::string> m_OriginalIdentifiers;

    SymbolTable m_SymbolTable;
    MinificationStats m_Stats;

    char m_NextVarName{'a'};
    int m_VarCounter{0};

    void initBuiltins();
    bool isBuiltin(const std::string& name) const;
    bool isTypeQualifier(TokenType type) const;
    bool isType(TokenType type) const;
    std::string getNextVarName();
    void collectIdentifiers();
    void collectProtectedIdentifiers();
    void collectOriginalIdentifiers();
    bool needsSpaceBetween(const Token& prev, const Token& curr);

    void buildSymbolTable();
    std::string generateOutput();

    inline const MinificationStats& getStats() const { return m_Stats; }

public:
    Minifier(const std::vector<Token>& tokens);
    std::string minify();
    void printRenamings();
    inline void setOriginalSize(size_t size) { m_Stats.setOriginalSize(size); }


    void printStats();
    void printDeadCode();
};


#endif //MINIFIER_HPP
