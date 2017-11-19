#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <exception>

#include "ast.hpp"

struct Symbol
{
public:
    Symbol();
    std::string m_ID;
    Type m_type;
};

//Note that here scope refers to any scope, whether {} or IF, IF-ELSE statements
typedef std::map<std::string, Symbol> SymbolScope;

class SymbolTable
{
public:
    SymbolTable();
    int enterScope(); //Returns new scope number
    int exitScope(); //Returns new scope number
    SymbolScope& currentScope();
    int currentScopeNum();

    bool pushElement(Symbol element);
    bool findElement(std::string& _ID);
    Symbol getElement(std::string& _ID);

private:
    int m_currentScopeNum;
    std::vector<SymbolScope> m_symbolTable;
};

#endif

