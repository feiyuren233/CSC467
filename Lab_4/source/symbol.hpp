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

    std::ostream& write(std::ostream& os) const;

private:
    int m_enumGivenType;
    int m_baseType;
    int m_vecSize;

    static int getBaseType(int _type, int vec_size);
};

class Symbol
{
public:
    enum specialDes {
        NORMAL,
        RESULT,
        ATTRIBUTE,
        UNIFORM
    };

    Symbol();
    Symbol(bool _isConst, const std::string& _id, Type _type, specialDes special = NORMAL);

    bool isConst() const {return m_isConst;}
    std::string id() const {return m_ID;}
    Type type() const {return m_type;}

    bool isSpecial() const { return m_specialDesignation != NORMAL; }
    specialDes getSpecialDesignation() const { return m_specialDesignation; }

    std::ostream& write(std::ostream& os) const;

    void setUniqueID(const std::string& unique_id) { m_uniqueID = unique_id; }
    std::string uniqueID() { return m_uniqueID; }

private:
    bool m_isConst;
    std::string m_ID;
    Type m_type;
    specialDes m_specialDesignation;

    std::string m_uniqueID;
};

class Scope;
typedef const Scope* ScopeID;
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

    bool findElementInStack(const std::string& _ID);
    bool findElementInCurrentScope(const std::string& _ID);
    void pushElement(Symbol element); //Overwrites an element if exists
    Symbol getElementInStack(const std::string& _ID); //May throw if element not found, call findElement first

    bool findSpecial(const std::string& _ID);
    Symbol getSpecial(const std::string& _ID); //May throw if _ID is not a pre-defined variable, call findSpecial first

private:
    ScopeID m_currentScopeID;
    std::vector<IDScopePair> m_currentScopeStack;
    SymbolTableCache m_cache;

    static SymbolScope m_preDefinedSpecials;
};

std::ostream& operator<<(std::ostream& os, const Type type);
std::ostream& operator<<(std::ostream& os, const Symbol symbol);

#endif

