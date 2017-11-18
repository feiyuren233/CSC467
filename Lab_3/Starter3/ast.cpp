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

Scope::Scope(Declarations *declarations, Statements *statements)
        :Node(), m_declarations(declarations), m_statements(statements) {}

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
        :Node(), m_declarations(declarations), m_declaration(declaration) {}

Declarations::~Declarations() {
    delete m_declarations;
    delete m_declaration;
}

std::ostream& Declarations::write(std::ostream &os) const {
    return os << m_declarations << m_declaration;
}

Declaration::Declaration(bool isConst, Type* type, const std::string& _ID, Expression* expression)
        :Node(), m_isConst(isConst), m_type(type), m_ID(_ID), m_expression(expression) {}

Declaration::~Declaration() {
    delete m_type;
    delete m_expression;
}

std::ostream& Declaration::write(std::ostream &os) const {
    return os << std::endl << "    "
              << (m_isConst ? "const " : "") << m_type << " " << m_ID << " " << m_expression;
}



Statements::Statements() {}
Statements::~Statements() {}
std::ostream& Statements::write(std::ostream &os) const {
    return os << "some_statement";
}

Expression::Expression() {}
Expression::~Expression() {}
std::ostream& Expression::write(std::ostream &os) const {
    return os << "some_expression";
}

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

std::ostream& operator<<(std::ostream& os, const Node* node) {
    return node? node->write(os) : os;
}

void ast_free(Node *ast) {
    delete ast;
}

void ast_print(Node * ast) {
    std::cout << ast << std::endl;
}
