#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP
#include <string>
#include <unordered_map>
#include <vector>

enum class SymbolKind
{
    VARIABLE, FUNCTION, UNIFORM, PARAMETER
};

struct Symbol
{
    std::string name;
    SymbolKind kind;
    bool isDeclared;
    bool isUsed;
    int declarationLine;
    std::vector<int> usageLines;

    Symbol() : isDeclared{false}, isUsed{false}, declarationLine{0}
    {
    }
};

class SymbolTable
{
private:
    std::unordered_map<std::string, Symbol> m_Symbols;

public:
    void declareSymbol(const std::string& name, SymbolKind kind, int line);
    void useSymbol(const std::string& name, int line);
    bool isSymbolUsed(const std::string& name) const;
    std::vector<std::string> getUnusedSymbols() const;
    int getUnusedCount() const;

    void print() const;
};


#endif //SYMBOLTABLE_HPP
