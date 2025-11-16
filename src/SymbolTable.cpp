#include "SymbolTable.hpp"

#include <iostream>

void SymbolTable::declareSymbol(const std::string& name, SymbolKind kind, int line)
{
    Symbol& sym{m_Symbols[name]};
    sym.name = name;
    sym.kind = kind;
    sym.isDeclared = true;
    sym.declarationLine = line;
}

void SymbolTable::useSymbol(const std::string& name, int line)
{
    if (m_Symbols.find(name) != m_Symbols.end())
    {
        m_Symbols[name].isUsed = true;
        m_Symbols[name].usageLines.push_back(line);
    }
    else
    {
        Symbol& sym{m_Symbols[name]};
        sym.name = name;
        sym.isUsed = true;
        sym.usageLines.push_back(line);
    }
}

bool SymbolTable::isSymbolUsed(const std::string& name) const
{
    auto it{m_Symbols.find(name)};
    return it != m_Symbols.end() && it->second.isUsed;
}

std::vector<std::string> SymbolTable::getUnusedSymbols() const
{
    std::vector<std::string> unused;
    for (const auto& pair : m_Symbols)
    {
        if (pair.second.isDeclared && !pair.second.isUsed)
            unused.push_back(pair.second.name);
    }
    return unused;
}

int SymbolTable::getUnusedCount() const
{
    int cnt{0};
    for (const auto& pair : m_Symbols)
    {
        if (pair.second.isDeclared && !pair.second.isUsed)
            ++cnt;
    }
    return cnt;
}

void SymbolTable::print() const
{
    auto unused{getUnusedSymbols()};
    if (unused.empty())
    {
        std::cout << "No unused symbols detected\n";
        return;
    }
    std::cout << "Dead code:\n" << "Found: " << unused.size() << " unused symbols\n";
    for (const auto& name : unused)
    {
        const Symbol& sym{m_Symbols.at(name)};
        std::cout << "Line " << sym.declarationLine << ": ";
        switch (sym.kind)
        {
        case SymbolKind::VARIABLE:
            std::cout << "variable ";
            break;
        case SymbolKind::FUNCTION:
            std::cout << "function ";
            break;
        case SymbolKind::UNIFORM:
            std::cout << "uniform ";
            break;
        case SymbolKind::PARAMETER:
            std::cout << "parameter ";
            break;
        }
        std::cout << "'" << name << "' is never used\n";
    }
}


