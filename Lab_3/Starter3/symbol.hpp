#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <exception>

class Type
{
public:
    Type(int _type, int vec_size = 1);

    int enumGivenType() const {return m_enumGivenType;}
    int baseType() const {return m_baseType;}
    int vecSize() const {return m_vecSize;}

private:
    int m_enumGivenType;
    int m_baseType;
    int m_vecSize;

    static int getBaseType(int _type, int vec_size);
};

class Symbol
{
public:
    Symbol();
    Symbol(bool _isConst, std::string& _id, Type _type);

    bool isConst() const {return m_isConst;}
    std::string id() const {return m_ID;}
    Type type() const {return m_type;}

private:
    bool m_isConst;
    std::string m_ID;
    Type m_type;
};

class Scope;
typedef Scope* ScopeID;
typedef std::map<std::string, Symbol> SymbolScope;
typedef std::map<ScopeID, SymbolScope> SymbolTableCache;
typedef std::pair<ScopeID, SymbolScope> IDScopePair;

class SymbolTable
{
public:
    SymbolTable();
    void enterScope(ScopeID scope_id);
    void exitScope();
    SymbolScope& currentScope(); //May throw if no scope has been entered yet

    bool findElementInStack(std::string& _ID);
    bool findElementInCurrentScope(std::string& _ID);
    void pushElement(Symbol element); //Overwrites an element if exists
    Symbol getElementInStack(std::string& _ID); //May throw if element not found, call findElement first

private:
    ScopeID m_currentScopeID;
    std::vector<IDScopePair> m_currentScopeStack;
    SymbolTableCache m_cache;
};

#endif

