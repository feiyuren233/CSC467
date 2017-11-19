#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "symbol.hpp"
//This is here because otherwise we get 'Node does not name a type' issue since 'parser.h' doesn't include 'ast.hpp'
#include "ast.hpp"
#include "parser.h"

Type::Type(int _type, int vec_size)
        :m_enumGivenType(_type), m_vecSize(vec_size), m_baseType(getBaseType(_type, vec_size)) {}

int Type::getBaseType(int _type, int vec_size) {
    if (vec_size == 1)
        return _type;
    else {
        switch (_type) {
            case VEC_T:
                return FLOAT_T;
            case BVEC_T:
               return BOOL_T;
            case IVEC_T:
                return INT_T;
            default:
                throw std::runtime_error("Type constructed with unknown _type: " + std::to_string(_type));
        }
    }
}

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