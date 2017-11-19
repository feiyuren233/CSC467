#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <iostream>
#include <map>
#include <exception>

#include "ast.hpp"
#include "common.h"
#include "parser.h"

#define DEBUG_PRINT_TREE 0

Node* ast = nullptr;

int Node::m_writeScopeLevel = 0;
int Node::m_writeIfLevel = 0;

void Node::enterScope() const {
    m_writeScopeLevel++;
}

void Node::exitScope() const {
    m_writeScopeLevel--;
}

void Node::enterIf() const {
    m_writeIfLevel++;
}

void Node::exitIf() const {
    m_writeIfLevel--;
}

std::string Node::indent(int relative) const {
    std::string ret;
    for (int i = m_writeScopeLevel + m_writeIfLevel + relative - 1; i > 0; i--) {
        ret += "    ";
    }
    return ret;
}


// Scope-------------------------------------------
Scope::Scope(Declarations *declarations, Statements *statements)
        :m_declarations(declarations), m_statements(statements) {}

Scope::~Scope() {
    delete m_declarations;
    delete m_statements;
}

std::ostream& Scope::write(std::ostream& os) const {
    enterScope();
    os << std::endl << indent(0) << "SCOPE ("
       << std::endl << indent(0) << "DECLARATIONS (" << m_declarations << ")"
       << std::endl << indent(0) << "STATEMENTS (" << m_statements << ")";
    exitScope();
    return os;
}


// Declarations------------------------------------
Declarations::Declarations(Declarations *declarations, Declaration *declaration)
        :m_declarations(declarations), m_declaration(declaration) {}

Declarations::~Declarations() {
    delete m_declarations;
    delete m_declaration;
}

std::ostream& Declarations::write(std::ostream &os) const {
    return os << m_declarations << m_declaration;
}


// Statements--------------------------------------
Statements::Statements(Statements* statements, Statement* statement)
        :m_statements(statements), m_statement(statement) {}

Statements::~Statements() {
    delete m_statements;
    delete m_statement;
}

std::ostream& Statements::write(std::ostream &os) const {
    return os << m_statements << m_statement;
}


// Declaration-------------------------------------
Declaration::Declaration(bool isConst, Type* type, const std::string& _ID, Expression* expression)
        :Node(), m_isConst(isConst), m_type(type), m_ID(_ID), m_expression(expression) {}

Declaration::~Declaration() {
    delete m_type;
    delete m_expression;
}

std::ostream& Declaration::write(std::ostream &os) const {
    return os << std::endl << indent(1)
              << (m_isConst ? "const " : "") << m_type << " " << m_ID << " " << m_expression;
}


// Statement---------------------------------------
Statement::Statement(Scope *scope)
        :m_scope(scope),
         m_variable(nullptr), m_expression(nullptr), m_statement(nullptr), m_elseStatement(nullptr) {}

Statement::Statement(Variable *variable, Expression *expression)
        :m_variable(variable), m_expression(expression),
         m_scope(nullptr), m_statement(nullptr), m_elseStatement(nullptr) {}

Statement::Statement(Expression *expression, Statement *statement, Statement *else_statement)
        :m_expression(expression), m_statement(statement), m_elseStatement(else_statement),
         m_scope(nullptr), m_variable(nullptr) {}

Statement::~Statement() {
    delete m_scope;
    delete m_variable;
    delete m_expression;
    delete m_statement;
    delete m_elseStatement;
}

std::ostream& Statement::write(std::ostream &os) const {
    if (!(m_scope || m_variable || m_expression || m_statement || m_elseStatement))
        return os;
    else if (m_scope)
        return os << m_scope;
    else if (m_variable && m_expression)
        return os << std::endl << indent(1) << m_variable << " = " << m_expression;
    else if (m_expression && m_statement) {
        enterIf();
        os << std::endl << indent(0) << "IF ( " << m_expression << " )"
           << m_statement;
        if (m_elseStatement) {
            os << std::endl << indent(0) << "ELSE"
               << m_elseStatement;
        }
        exitIf();
        return os;
    } else throw 0; //TODO: See about throwing an informative exception here
}


// Type--------------------------------------------
Type::Type(int _type, int vec_size)
        :m_enumGivenType(_type), m_vecSize(vec_size) {
    if (m_vecSize == 1)
        m_primaryType = m_enumGivenType;
    else {
        switch (_type) {
            case VEC_T:
                m_primaryType = FLOAT_T;
                break;
            case BVEC_T:
                m_primaryType = BOOL_T;
                break;
            case IVEC_T:
                m_primaryType = INT_T;
                break;
            default:
                throw 0;  //TODO: See about throwing an informative exception here
        }
    }
}

std::ostream& Type::write(std::ostream &os) const {
    std::map<int, std::string> type_enum_to_string{
            {FLOAT_T, "float"}, {INT_T, "int"}, {BOOL_T, "bool"},
            {VEC_T, "vec"}, {IVEC_T, "ivec"}, {BVEC_T, "bvec"}};

    return os << type_enum_to_string[m_enumGivenType] << ((m_vecSize > 1)? std::to_string(m_vecSize) : "");
}


// Expression--------------------------------------
OperationExpression::OperationExpression(int _op, Expression *rhs)
        :Expression(rhs->isConstExpr()), m_lhs(nullptr), m_rhs(rhs), m_operator(_op) {}

OperationExpression::OperationExpression(Expression *lhs, int _op, Expression *rhs)
        :Expression(lhs->isConstExpr() && rhs->isConstExpr()), m_lhs(lhs), m_rhs(rhs), m_operator(_op) {}

OperationExpression::~OperationExpression() {
    delete m_lhs;
    delete m_rhs;
}

std::ostream& OperationExpression::write(std::ostream &os) const {
    std::map<int, std::string> op_to_string{
            {AND, "&&"}, {OR, "||"}, {EQ, "=="}, {NEQ, "!="}, {LEQ, "<="}, {GEQ, ">="},
            {'<', "<"}, {'>', ">"}, {'+', "+"}, {'-', "-"}, {'*', "*"}, {'/', "/"}, {'^', "^"},
            {UMINUS, "-"}, {'!', "!"}};

    return os << m_lhs << (m_lhs? " " : "") << op_to_string[m_operator] << (m_lhs? " " : "") << m_rhs;
}

LiteralExpression::LiteralExpression(bool val)
        :Expression(true), m_valBool(val), m_isBool(true), m_isInt(false), m_isFloat(false) {}

LiteralExpression::LiteralExpression(int val)
        :Expression(true), m_valInt(val), m_isBool(false), m_isInt(true), m_isFloat(false) {}

LiteralExpression::LiteralExpression(float val)
        :Expression(true), m_valFloat(val), m_isBool(false), m_isInt(true), m_isFloat(false) {}

std::ostream& LiteralExpression::write(std::ostream& os) const {
    if (m_isBool)
        return os << (m_valBool? "true" : "false");
    else if (m_isInt)
        return os << m_valInt;
    else if (m_isFloat)
        return os << m_valFloat;
}

OtherExpression::OtherExpression(Expression* expression)
        :Expression(expression->isConstExpr()), m_expression(expression),
         m_variable(nullptr), m_arguments(nullptr), m_type(nullptr), m_func(-1) {}

OtherExpression::OtherExpression(Variable* variable)
        :Expression(false), m_variable(variable),
         m_expression(nullptr), m_arguments(nullptr), m_type(nullptr), m_func(-1) {}

OtherExpression::OtherExpression(Type* _type, Arguments* arguments)
        :Expression(false), m_type(_type), m_arguments(arguments),
         m_expression(nullptr), m_variable(nullptr), m_func(-1) {}

OtherExpression::OtherExpression(int func, Arguments* arguments)
        :Expression(false), m_func(func), m_arguments(arguments),
         m_expression(nullptr), m_variable(nullptr), m_type(nullptr) {}

OtherExpression::~OtherExpression() {
    delete m_expression;
    delete m_variable;
    delete m_arguments;
    delete m_type;
}

std::ostream& OtherExpression::write(std::ostream &os) const {
    if (m_expression)
        return os << "( " << m_expression << " )";
    else if (m_variable)
        return os << m_variable;
    else if (m_type)
        return os << m_type << " ( " << m_arguments << " ) ";
    else {
        std::map<int, std::string> func_to_string{ {0, "dp3"}, {1, "lit"}, {2, "rsq"}};
        return os << func_to_string[m_func] << " ( " << m_arguments << " ) ";
    }
}

// Variable----------------------------------------
Variable::Variable(const std::string& _ID)
        :m_ID(_ID), m_index(0), m_isIndexPresent(false) {}

Variable::Variable(const std::string& _ID, int index)
        :m_ID(_ID), m_index(index), m_isIndexPresent(true) {}

std::ostream& Variable::write(std::ostream &os) const {
    return os << m_ID << (m_isIndexPresent? ("[" + std::to_string(m_index) + "]") : "");
}


// Arguments---------------------------------------
Arguments::Arguments(Arguments* arguments, Expression* expression)
        :m_arguments(arguments), m_expression(expression) {}

Arguments::~Arguments() {
    delete m_arguments;
    delete m_expression;
}

std::ostream& Arguments::write(std::ostream &os) const {
    return os << m_arguments << ((m_arguments && m_expression)? ", " : "") << m_expression;
}




std::ostream& operator<<(std::ostream& os, const Node* node) {
    return node? node->write(os) : os;
}

void ast_free(Node *ast) {
    delete ast;
}

void ast_print(Node * ast) {
    std::cout << ast << std::endl;
}
