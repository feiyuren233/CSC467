
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
class Declaration;
class Statements;
class Statement;
class Expression;
class Type;

class Node
{
public:
    virtual ~Node() = default; //Analogous to free()
    virtual std::ostream& write(std::ostream& os) const = 0;
};

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

class Statements : public Node
{
public:
    Statements();
    virtual ~Statements();
    virtual std::ostream& write(std::ostream& os) const;
};

class Expression : public Node
{
public:
    Expression();
    virtual ~Expression();
    virtual std::ostream& write(std::ostream& os) const;
};

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


std::ostream& operator<<(std::ostream& os, const Node* node);

void ast_free(Node *ast);
void ast_print(Node * ast);

#endif /* AST_H_ */
