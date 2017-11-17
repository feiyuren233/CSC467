#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <iostream>

#include "ast.hpp"
#include "common.h"
#include "parser.h"

#define DEBUG_PRINT_TREE 0

Node* ast = nullptr;

Node::~Node() {}

std::ostream& operator<<(std::ostream& os, const Node* node) {
    return node? node->write(os) : os;
}

Scope::Scope(Declarations *declarations, Statements *statements)
        :Node(),
         m_declarations(declarations), m_statements(statements) {}

Scope::~Scope() {
    delete m_declarations;
    delete m_statements;
}

std::ostream& Scope::write(std::ostream& os) const {
    return os << "SCOPE (" << m_declarations << ") (" << m_statements << ")" << std::endl;
}

Declarations::Declarations(Declarations *declarations, Declaration *declaration)
        :Node(),
         m_declarations(declarations), m_declaration(declaration) {}

Declarations::~Declarations() {
    delete m_declarations;
    delete m_declaration;
}

std::ostream& Declarations::write(std::ostream &os) const {
    return os << "DECLARATIONS (" << m_declarations << m_declaration << ")" << std::endl;
}

Declaration::Declaration(bool isConst, Type type, const std::string& _ID, Expression* expression)
        :Node(), m_isConst(isConst), m_type(type), m_ID(_ID), m_expression(expression) {}

Declaration::~Declaration() {
    delete m_expression;
}

std::ostream& Declaration::write(std::ostream &os) const {
    return os << (m_isConst ? "const " : "") << m_type.m_type << " " << m_ID << " " << m_expression << std::endl;
}



Statements::Statements() {}
Statements::~Statements() {}
std::ostream& Statements::write(std::ostream &os) const {
    return os << "STATEMENTS";
}

Expression::Expression() {}
Expression::~Expression() {}
std::ostream& Expression::write(std::ostream &os) const {
    return os << "EXPRESSION";
}


void ast_free(Node *ast) {
    delete ast;
}

void ast_print(Node * ast) {
    std::cout << ast << std::endl;
}
