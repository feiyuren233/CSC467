
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>
#include <string>
#include <iostream>
#include "symbol.hpp"

// Dummy node just so everything compiles, create your own node/nodes
//
// The code provided below is an example ONLY. You can use/modify it,
// but do not assume that it is correct or complete.
//
// There are many ways of making AST nodes. The approach below is an example
// of a descriminated union. If you choose to use C++, then I suggest looking
// into inheritance.
class Node;
extern Node* ast;

// forward declare
class Scope;
class Declarations;
class Statements;
class Declaration;
class Statement;
class TypeNode;
class HasType;
class Expression;
class Variable;
class Arguments;


class Node
{
public:
    virtual ~Node() = default; //Analogous to free()
    virtual std::ostream& write(std::ostream& os) const { return os; }

    bool semanticallyCorrect() { return !m_semanticErrorFound; }
    virtual std::ostream& populateSymbolTableAndCheckErrors(std::ostream &os) { return os; }

protected:
    void enterScope(const Scope* scope = nullptr) const;
    void exitScope() const;

    // Facilities for printing AST
    static int m_writeScopeLevel;
    static int m_writeIfLevel;
    void enterIf() const;
    void exitIf() const;
    std::string indent(int relative = 0) const;

    // Facilities for semantic checking
    static SymbolTable m_symbolTable;
    static bool m_semanticErrorFound;
    bool foundSemanticError() { m_semanticErrorFound = true; }
};

// Scope-------------------------------------------
class Scope : public Node
{
public:
    Scope(Declarations* declarations, Statements* statements);
    virtual ~Scope();
    virtual std::ostream& write(std::ostream& os) const;

    virtual std::ostream& populateSymbolTableAndCheckErrors(std::ostream &os);

private:
    Declarations* m_declarations;
    Statements* m_statements;
};

// Declarations------------------------------------
class Declarations : public Node
{
public:
    Declarations(Declarations* declarations = nullptr, Declaration* declaration = nullptr);
    virtual ~Declarations();
    virtual std::ostream& write(std::ostream& os) const;

    virtual std::ostream& populateSymbolTableAndCheckErrors(std::ostream &os);

private:
    Declarations* m_declarations;
    Declaration* m_declaration;
};

// Statements--------------------------------------
class Statements : public Node
{
public:
    Statements(Statements* statements = nullptr, Statement* statement = nullptr);
    virtual ~Statements();
    virtual std::ostream& write(std::ostream& os) const;

    virtual std::ostream& populateSymbolTableAndCheckErrors(std::ostream &os);

private:
    Statements* m_statements;
    Statement* m_statement;
};

// Declaration-------------------------------------
class Declaration : public Node
{
public:
    Declaration(bool isConst, TypeNode* typeNode, const std::string& _ID, Expression* expression= nullptr);
    virtual ~Declaration();
    virtual std::ostream& write(std::ostream& os) const;

    virtual std::ostream& populateSymbolTableAndCheckErrors(std::ostream &os);

private:
    bool m_isConst;
    TypeNode* m_typeNode;
    const std::string m_ID;
    Expression* m_expression;
};

// Statement---------------------------------------
class Statement : public Node
{
public:
    Statement(Scope* scope = nullptr);
    Statement(Variable* variable, Expression* expression);
    Statement(Expression* expression, Statement* statement, Statement* else_statement= nullptr);
    virtual ~Statement();
    virtual std::ostream& write(std::ostream& os) const;

    virtual std::ostream& populateSymbolTableAndCheckErrors(std::ostream &os);

private:
    Scope* m_scope;
    Variable* m_variable;
    Expression* m_expression;
    Statement* m_statement;
    Statement* m_elseStatement;
};

// TypeNode----------------------------------------
class TypeNode : public Node
{
public:
    TypeNode(int _type, int vec_size = 1);
    virtual std::ostream& write(std::ostream& os) const;

    Type type() { return m_type; }
private:
    Type m_type;
};

// To aid in semantics checking, meant to be inherited from by any Node that has a type
class HasType : public Node
{
public:
    HasType();
    virtual ~HasType() = default;

protected:
    void setType(Type type);
    void setType(int _type, int vec_size = 1);
    Type m_type;
};

// Expression--------------------------------------
class Expression : public HasType
{
public:
    Expression(bool is_constexpr = false);
    virtual ~Expression() = default;

    bool isConstExpr() { return m_isConstExpr;}

protected:
    bool m_isConstExpr;
};

class OperationExpression : public Expression
{
public:
    OperationExpression(int _op, Expression* rhs);
    OperationExpression(Expression* lhs, int _op, Expression* rhs);
    virtual ~OperationExpression();
    virtual std::ostream& write(std::ostream& os) const;

private:
    Expression* m_lhs;
    Expression* m_rhs;
    int m_operator;
};

class LiteralExpression : public Expression
{
public:
    explicit LiteralExpression(bool val);
    explicit LiteralExpression(int val);
    explicit LiteralExpression(float val);
    virtual std::ostream& write(std::ostream& os) const;

private:
    bool m_isBool;
    bool m_isInt;
    bool m_isFloat;

    union {
        bool m_valBool;
        int m_valInt;
        float m_valFloat;
    };
};

class OtherExpression : public Expression
{
public:
    OtherExpression(Expression* expression);
    OtherExpression(Variable* variable);
    OtherExpression(TypeNode* _type, Arguments* arguments);
    OtherExpression(int func, Arguments* arguments);
    virtual ~OtherExpression();
    virtual std::ostream& write(std::ostream& os) const;

    virtual std::ostream& populateSymbolTableAndCheckErrors(std::ostream &os);

private:
    Expression* m_expression;
    Variable* m_variable;
    Arguments* m_arguments;
    TypeNode* m_typeNode;
    int m_func;
};

// Variable----------------------------------------
class Variable : public HasType
{
public:
    Variable(const std::string& _ID);
    Variable(const std::string& _ID, int index);
    virtual ~Variable() = default;
    virtual std::ostream& write(std::ostream& os) const;

    std::string id() const {return m_ID;}
    virtual std::ostream& populateSymbolTableAndCheckErrors(std::ostream &os);

private:
    const std::string m_ID;
    int m_index;
    bool m_isIndexPresent;
};

// Arguments---------------------------------------
class Arguments : public Node
{
public:
    Arguments(Arguments* arguments = nullptr, Expression* expression = nullptr);
    virtual ~Arguments();
    virtual std::ostream& write(std::ostream& os) const;

    virtual std::ostream& populateSymbolTableAndCheckErrors(std::ostream &os);

private:
    Arguments* m_arguments;
    Expression* m_expression;
};



std::ostream& operator<<(std::ostream& os, const Node* node);

void ast_free(Node *ast);
void ast_print(Node * ast);

#endif /* AST_H_ */
