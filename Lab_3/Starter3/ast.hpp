
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>
#include <iostream>

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
class Type;
class Expression;
class Variable;
class Arguments;


class Node
{
public:
    virtual ~Node() = default; //Analogous to free()
    virtual std::ostream& write(std::ostream& os) const = 0;

protected:
    static int m_writeScopeLevel;
    static int m_writeIfLevel;
    void enterScope() const;
    void exitScope() const;
    void enterIf() const;
    void exitIf() const;
    std::string indent(int relative = 0) const;
};

// Scope-------------------------------------------
class Scope : public Node
{
public:
    Scope(Declarations* declarations, Statements* statements);
    virtual ~Scope();
    virtual std::ostream& write(std::ostream& os) const;

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

private:
    Statements* m_statements;
    Statement* m_statement;
};

// Declaration-------------------------------------
class Declaration : public Node
{
public:
    Declaration(bool isConst, Type* type, const std::string& _ID, Expression* expression= nullptr);
    virtual ~Declaration();
    virtual std::ostream& write(std::ostream& os) const;

private:
    bool m_isConst;
    Type* m_type;
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

private:
    Scope* m_scope;
    Variable* m_variable;
    Expression* m_expression;
    Statement* m_statement;
    Statement* m_elseStatement;
};

// Type--------------------------------------------
class Type : public Node
{
public:
    Type(int _type, int vec_size = 1);
    virtual std::ostream& write(std::ostream& os) const;

private:
    int m_enumGivenType;
    int m_primaryType;
    int m_vecSize;
};

// Expression--------------------------------------
class Expression : public Node
{
public:
    Expression();
    virtual ~Expression();
    virtual std::ostream& write(std::ostream& os) const;
};

// Variable----------------------------------------
class Variable : public Node
{
public:
    Variable(const std::string& ID);
    Variable(const std::string& ID, int index);
    virtual ~Variable() = default;
    virtual std::ostream& write(std::ostream& os) const;

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

private:
    Arguments* m_arguments;
    Expression* m_expression;
};



std::ostream& operator<<(std::ostream& os, const Node* node);

void ast_free(Node *ast);
void ast_print(Node * ast);

#endif /* AST_H_ */
