#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "symbol.hpp"
#include "parser.h"

Symbol::Symbol() :m_type(ANY_T) {}

SymbolTable::SymbolTable() :m_currentScopeNum(0) {}

int SymbolTable::enterScope() {
    m_symbolTable.push_back(SymbolScope());
    return ++m_currentScopeNum;
}

int SymbolTable::exitScope() {
    m_symbolTable.pop_back();
    return --m_currentScopeNum;
}

SymbolScope& SymbolTable::currentScope() {
    return m_symbolTable.back(); //may throw if no scope has been entered yet
}

int SymbolTable::currentScopeNum() {
    return m_currentScopeNum;
}

bool SymbolTable::pushElement(Symbol element) {
    if (currentScope().find(element.m_ID) != currentScope().end())
        return false; //Element with this name already present
    else {
        currentScope()[element.m_ID] = element;
        return true;
    }
}

bool SymbolTable::findElement(std::string& _ID) {
    for (auto scope = m_symbolTable.rbegin(); scope != m_symbolTable.rend(); scope++) {
        if (scope->find(_ID) != scope->end())
            return true;
    }
    return false;
}

Symbol SymbolTable::getElement(std::string &_ID) { //may throw if element not found, call findElement first
    for (auto scope = m_symbolTable.rbegin(); scope != m_symbolTable.rend(); scope++) {
        if (scope->find(_ID) != scope->end())
            return scope->find(_ID)->second;
    }
    throw std::runtime_error("SymbolTable::getElement: Symbol " + _ID + " not found in symbol table");
}