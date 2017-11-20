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

std::ostream& Type::write(std::ostream &os) const {
    std::map<int, std::string> type_enum_to_string{
            {FLOAT_T, "float"}, {INT_T, "int"}, {BOOL_T, "bool"},
            {VEC_T, "vec"}, {IVEC_T, "ivec"}, {BVEC_T, "bvec"},
            {ANY_T, "ANY"}};

    return os << type_enum_to_string[m_enumGivenType]
              << ((m_vecSize > 1)? std::to_string(m_vecSize) : "");
}

Symbol::Symbol()
        :m_isConst(false), m_type(ANY_T), m_specialDesignation(NORMAL) {}

Symbol::Symbol(bool _isConst, const std::string &_id, Type _type, specialDes special)
        :m_isConst(_isConst), m_ID(_id), m_type(_type), m_specialDesignation(special) {}

std::ostream& Symbol::write(std::ostream &os) const {
    return os << (m_isConst ? "const " : "") << m_type << " " << m_ID;
}

//SymbolTable ----------------------------------------------------
SymbolTable::SymbolTable()
        :m_currentScopeID(nullptr) {}

void SymbolTable::enterScope(ScopeID scope_id) {
    m_currentScopeID = scope_id;
    if (m_cache.find(m_currentScopeID) == m_cache.end())
        m_cache[m_currentScopeID] = SymbolScope();
    m_currentScopeStack.push_back(IDScopePair(m_currentScopeID, m_cache[m_currentScopeID]));
}

void SymbolTable::exitScope() {
    m_cache[m_currentScopeID] = m_currentScopeStack.back().second;
    m_currentScopeStack.pop_back();
    m_currentScopeID = (m_currentScopeStack.empty()? nullptr : m_currentScopeStack.back().first);
}

SymbolScope& SymbolTable::currentScope() {
    return m_currentScopeStack.back().second;
}

bool SymbolTable::findElementInStack(const std::string& _ID) {
    for (auto id_scope = m_currentScopeStack.rbegin(); id_scope != m_currentScopeStack.rend(); id_scope++) {
        if (id_scope->second.find(_ID) != id_scope->second.end())
            return true;
    }
    return (m_preDefinedSpecials.find(_ID) != m_preDefinedSpecials.end());
}

bool SymbolTable::findElementInCurrentScope(const std::string &_ID) {
    return (currentScope().find(_ID) != currentScope().end());
}

void SymbolTable::pushElement(Symbol element) {
    currentScope()[element.id()] = element;
}

Symbol SymbolTable::getElementInStack(const std::string &_ID) {
    for (auto id_scope = m_currentScopeStack.rbegin(); id_scope != m_currentScopeStack.rend(); id_scope++) {
        if (id_scope->second.find(_ID) != id_scope->second.end())
            return id_scope->second.at(_ID);
    }
    if (m_preDefinedSpecials.find(_ID) != m_preDefinedSpecials.end())
        return m_preDefinedSpecials.find(_ID)->second;
    throw std::runtime_error("SymbolTable::getElement: Symbol " + _ID + " not found in symbol table");
}

bool SymbolTable::findSpecial(const std::string &_ID) {
    return m_preDefinedSpecials.find(_ID) != m_preDefinedSpecials.end();
}

Symbol SymbolTable::getSpecial(const std::string &_ID) {
    return m_preDefinedSpecials[_ID];
}


SymbolScope SymbolTable::m_preDefinedSpecials {
        {"gl_FragColor", Symbol(false, "gl_FragColor", Type(VEC_T, 4), Symbol::RESULT)},
        {"gl_FragDepth", Symbol(false, "gl_FragDepth", Type(BOOL_T), Symbol::RESULT)},
        {"gl_FragCoord", Symbol(false, "gl_FragCoord", Type(VEC_T, 4), Symbol::RESULT)},

        {"gl_TextCoord", Symbol(false, "gl_TextCoord", Type(VEC_T, 4), Symbol::ATTRIBUTE)},
        {"gl_Color", Symbol(false, "gl_Color", Type(VEC_T, 4), Symbol::ATTRIBUTE)},
        {"gl_Secondary", Symbol(false, "gl_Secondary", Type(VEC_T, 4), Symbol::ATTRIBUTE)},
        {"gl_FogFragCoord", Symbol(false, "gl_FogFragCoord", Type(VEC_T, 4), Symbol::ATTRIBUTE)},

        {"gl_Light_Half", Symbol(true, "gl_Light_Half", Type(VEC_T, 4), Symbol::UNIFORM)},
        {"gl_Light_Ambient", Symbol(true, "gl_Light_Ambient", Type(VEC_T, 4), Symbol::UNIFORM)},
        {"gl_Material_Shininess", Symbol(true, "gl_Material_Shininess", Type(VEC_T, 4), Symbol::UNIFORM)},
        {"env1", Symbol(true, "env1", Type(VEC_T, 4), Symbol::UNIFORM)},
        {"env2", Symbol(true, "env2", Type(VEC_T, 4), Symbol::UNIFORM)},
        {"env3", Symbol(true, "env3", Type(VEC_T, 4), Symbol::UNIFORM)}
};


std::ostream& operator<<(std::ostream& os, const Type type) {
    return type.write(os);
}
std::ostream& operator<<(std::ostream& os, const Symbol symbol) {
    return symbol.write(os);
}