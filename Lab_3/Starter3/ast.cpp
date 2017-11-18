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
    return os << "\nSCOPE ("
              << "\nDECLARATIONS (" << m_declarations << ") ("
              << "\nSTATEMENTS (" << m_statements << ")";
}

Declarations::Declarations(Declarations *declarations, Declaration *declaration)
        :Node(),
         m_declarations(declarations), m_declaration(declaration) {}

Declarations::~Declarations() {
    delete m_declarations;
    delete m_declaration;
}

std::ostream& Declarations::write(std::ostream &os) const {
    return os << m_declarations << m_declaration;
}

Declaration::Declaration(bool isConst, Type type, const std::string& _ID, Expression* expression)
        :Node(), m_isConst(isConst), m_type(type), m_ID(_ID), m_expression(expression) {}

Declaration::~Declaration() {
    delete m_expression;
}

std::ostream& Declaration::write(std::ostream &os) const {
    return os << std::endl << (m_isConst ? "const " : "") << m_type.m_type << " " << m_ID << " " << m_expression;
}



Statements::Statements() {}
Statements::~Statements() {}
std::ostream& Statements::write(std::ostream &os) const {
    return os << std::endl << "some_statement";
}

Expression::Expression() {}
Expression::~Expression() {}
std::ostream& Expression::write(std::ostream &os) const {
    return os << std::endl << "some_expression";
}


void ast_free(Node *ast) {
    delete ast;
}

void ast_print(Node * ast) {
    std::cout << ast << std::endl;
}
