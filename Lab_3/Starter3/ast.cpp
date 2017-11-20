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

void Node::setParserInfo(int line_num) {
    m_parserLineNum = line_num;
}

void Node::setParserInfo(int line_num, const char *text) {
    m_parserLineNum = line_num;
    //m_parserTextNum = line - text;
    //m_parserLine = std::string(line);
    m_parserText = std::string(text);
}

void Node::enterScope(const Scope* scope) const {
    m_writeScopeLevel++;
    m_symbolTable.enterScope(scope);
}

void Node::exitScope() const {
    m_writeScopeLevel--;
    m_symbolTable.exitScope();
}

// Facilities for printing AST
int Node::m_writeScopeLevel = 0;
int Node::m_writeIfLevel = 0;

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

// Facilities for semantic checking
SymbolTable Node::m_symbolTable;
bool Node::m_semanticErrorFound = false;

std::string Node::semanticErrorHeader() const {
    return "\n\n" + std::to_string(m_parserLineNum) + ": " + m_parserText + "\n";
}


// Scope-------------------------------------------
Scope::Scope(Declarations *declarations, Statements *statements)
        :m_declarations(declarations), m_statements(statements) {}

Scope::~Scope() {
    delete m_declarations;
    delete m_statements;
}

std::ostream& Scope::write(std::ostream& os) const {
    enterScope(this);
    os << std::endl << indent(0) << "(SCOPE"
       << std::endl << indent(0) << "(DECLARATIONS" << m_declarations << ")"
       << std::endl << indent(0) << "(STATEMENTS" << m_statements << ")";
    exitScope();
    return os;
}

std::ostream& Scope::populateSymbolTableAndCheckErrors(std::ostream &os) {
    enterScope(this);
    if (m_declarations) m_declarations->populateSymbolTableAndCheckErrors(os);
    if (m_statements) m_statements->populateSymbolTableAndCheckErrors(os);
    exitScope();
    return os;
}

std::ostream& Scope::populateTypeAndCheckErrors(std::ostream &os) {
    enterScope(this);
    if (m_declarations) m_declarations->populateTypeAndCheckErrors(os);
    if (m_statements) m_statements->populateTypeAndCheckErrors(os);
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

std::ostream& Declarations::populateSymbolTableAndCheckErrors(std::ostream &os) {
    if (m_declarations) m_declarations->populateSymbolTableAndCheckErrors(os);
    if (m_declaration) m_declaration->populateSymbolTableAndCheckErrors(os);
    return os;
}

std::ostream& Declarations::populateTypeAndCheckErrors(std::ostream &os) {
    if (m_declarations) m_declarations->populateTypeAndCheckErrors(os);
    if (m_declaration) m_declaration->populateTypeAndCheckErrors(os);
    return os;
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

std::ostream& Statements::populateSymbolTableAndCheckErrors(std::ostream &os) {
    if (m_statements) m_statements->populateSymbolTableAndCheckErrors(os);
    if (m_statement) m_statement->populateSymbolTableAndCheckErrors(os);
    return os;
}

std::ostream& Statements::populateTypeAndCheckErrors(std::ostream &os) {
    if (m_statements) m_statements->populateTypeAndCheckErrors(os);
    if (m_statement) m_statement->populateTypeAndCheckErrors(os);
    return os;
}


// Declaration-------------------------------------
Declaration::Declaration(bool isConst, TypeNode* type, const std::string& _ID, Expression* expression)
        :Node(), m_isConst(isConst), m_typeNode(type), m_ID(_ID), m_expression(expression) {}

Declaration::~Declaration() {
    delete m_typeNode;
    delete m_expression;
}

std::ostream& Declaration::write(std::ostream &os) const {
    return os << std::endl << indent(1)
              << "(DECLARATION " << (m_isConst ? "const " : "") << m_typeNode << " " << m_ID << " " << m_expression << ")";
}

std::ostream& Declaration::populateSymbolTableAndCheckErrors(std::ostream &os) {
    if (m_symbolTable.findElementInCurrentScope(m_ID)) {
        foundSemanticError();
        Symbol existing_symbol = m_symbolTable.getElementInStack(m_ID);
        os << semanticErrorHeader()
           << "In declaration: " << Symbol(m_isConst, m_ID, m_typeNode->type())
           << std::endl << "'" << m_ID << "'" << " already declared in this scope: "
           << existing_symbol;
    }
    else m_symbolTable.pushElement(Symbol(m_isConst, m_ID, m_typeNode->type()));
    return os;
}

std::ostream& Declaration::populateTypeAndCheckErrors(std::ostream &os) {
    if (m_expression) {
        m_expression->populateTypeAndCheckErrors(os);
        if (!exactEqual(m_expression->type(), m_typeNode->type())) {
            foundSemanticError();
            os << semanticErrorHeader()
               << "In declaration: " << this
               << std::endl << "Assignment expression of type '" << m_expression->type()
               << "' does not match variable's type '" <<  m_typeNode->type() << "'";
        }
    }
    return os;
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
    else if (m_variable && m_expression) //variable = expression
        return os << std::endl << indent(1) << "(ASSIGN " <<m_variable << " " << m_expression << ")";
    else if (m_expression && m_statement) { //if statement
        enterIf();
        os << std::endl << indent(0) << "IF (" << m_expression << " )"
           << m_statement;
        if (m_elseStatement) {
            os << std::endl << indent(0) << "ELSE"
               << m_elseStatement;
        }
        exitIf();
        return os;
    } else throw std::runtime_error("Statement::write: No correct sub-type of Statement found");
}

std::ostream& Statement::populateSymbolTableAndCheckErrors(std::ostream &os) {
    if (m_scope) m_scope->populateSymbolTableAndCheckErrors(os);
    if (m_variable) {
        m_variable->populateSymbolTableAndCheckErrors(os);
        if (m_symbolTable.findElementInStack(m_variable->id())) {
            Symbol symbol = m_symbolTable.getElementInStack(m_variable->id());
            if (symbol.isConst()) {
                foundSemanticError();
                os << semanticErrorHeader()
                   << "In assignment expression: " << m_variable << " = " << m_expression
                   << std::endl << "Cannot assign const-qualified variable '" << symbol << "'";
            }
        }
    }
    if (m_expression) m_expression->populateSymbolTableAndCheckErrors(os);
    if (m_statement) m_statement->populateSymbolTableAndCheckErrors(os);
    if (m_elseStatement) m_elseStatement->populateSymbolTableAndCheckErrors(os);
    return os;
}

std::ostream& Statement::populateTypeAndCheckErrors(std::ostream &os) {
    if (m_scope) m_scope->populateTypeAndCheckErrors(os);
    if (m_variable) m_variable->populateTypeAndCheckErrors(os);
    if (m_expression) m_expression->populateTypeAndCheckErrors(os);
    if (m_statement) m_statement->populateTypeAndCheckErrors(os);
    if (m_elseStatement) m_elseStatement->populateTypeAndCheckErrors(os);

    if (m_variable && m_expression) {//variable = expression
        if (!exactEqual(m_variable->type(), m_expression->type())) {
            foundSemanticError();
            os << semanticErrorHeader()
               << "In assignment expression: " << m_variable << " = " << m_expression
               << std::endl << "Assignment-expression '" << m_expression << "' of type '" << m_expression->type()
               << "' does not match variable's type '" <<  m_variable->type() << "'";
        }
    }
    else if (m_expression && m_statement) {//if statement
        if (!exactEqual(m_expression->type(), Type(BOOL_T))) {
            foundSemanticError();
            os << semanticErrorHeader()
               << "In if statement: IF (" << m_expression << ") "
               << std::endl << "The expression '" << m_expression << "' is not BOOL_T";
        }
    }
    return os;
}


// Type--------------------------------------------
TypeNode::TypeNode(int _type, int vec_size)
        :m_type(_type, vec_size) {}

std::ostream& TypeNode::write(std::ostream &os) const {
    return os << m_type;
}

HasType::HasType() :m_type(Type(ANY_T)) {}

void HasType::setType(Type type) {
    m_type = type;
}

void HasType::setType(int _type, int vec_size) {
    m_type = Type(_type, vec_size);
}


// Expression--------------------------------------
Expression::Expression(bool is_constexpr)
        :HasType(), m_isConstExpr(is_constexpr) {}


std::map<int, std::string> OperationExpression::m_op_to_string {
        {AND, "&&"}, {OR, "||"}, {EQ, "=="}, {NEQ, "!="}, {LEQ, "<="}, {GEQ, ">="},
        {'<', "<"}, {'>', ">"}, {'+', "+"}, {'-', "-"}, {'*', "*"}, {'/', "/"}, {'^', "^"},
        {UMINUS, "-"}, {'!', "!"}};

OperationExpression::OperationExpression(int _op, Expression *rhs)
        :Expression(rhs->isConstExpr()), m_lhs(nullptr), m_rhs(rhs), m_operator(_op) {}

OperationExpression::OperationExpression(Expression *lhs, int _op, Expression *rhs)
        :Expression(lhs->isConstExpr() && rhs->isConstExpr()), m_lhs(lhs), m_rhs(rhs), m_operator(_op) {}

OperationExpression::~OperationExpression() {
    delete m_lhs;
    delete m_rhs;
}

std::ostream& OperationExpression::write(std::ostream &os) const {
    return os << (m_lhs? "(BINARY " : "(UNARY ") << m_rhs->type() << " " << m_op_to_string[m_operator] << " " << m_lhs << (m_lhs? " " : "") << m_rhs << ")";
}

std::ostream& OperationExpression::populateTypeAndCheckErrors(std::ostream &os) {
    if (m_lhs) m_lhs->populateTypeAndCheckErrors(os);
    if (m_rhs) m_rhs->populateTypeAndCheckErrors(os);

    if (m_lhs && m_rhs) {
        if (!operationEqual(m_lhs->type(), m_operator, m_rhs->type())) {
        foundSemanticError();
        os << semanticErrorHeader()
           << "In expression: " << this
           << std::endl << "Type of right-hand-side operand '" << m_rhs->type()
           << "' does not match left-hand-side operand of type '" << m_lhs->type() << "'";
        setType(ANY_T);
        }
    }
    else if (!validUnary(m_operator, m_rhs->type())) {
        foundSemanticError();
        os << semanticErrorHeader()
           << "In unary expression: " << this
           << std::endl << "Type of operand '" << m_rhs->type()
           << "' is not valid for operator '" << m_op_to_string[m_operator] << "'";
        setType(ANY_T);
    }
    else setType(m_rhs->type());
    return os;
}


LiteralExpression::LiteralExpression(bool val)
        :Expression(true), m_valBool(val), m_isBool(true), m_isInt(false), m_isFloat(false) {}

LiteralExpression::LiteralExpression(int val)
        :Expression(true), m_valInt(val), m_isBool(false), m_isInt(true), m_isFloat(false) {}

LiteralExpression::LiteralExpression(float val)
        :Expression(true), m_valFloat(val), m_isBool(false), m_isInt(false), m_isFloat(true) {}

std::ostream& LiteralExpression::write(std::ostream& os) const {
    if (m_isBool)
        return os << (m_valBool? "true" : "false");
    else if (m_isInt)
        return os << m_valInt;
    else if (m_isFloat)
        return os << m_valFloat;
}

std::ostream& LiteralExpression::populateTypeAndCheckErrors(std::ostream &os) {
    if (m_isBool)
        setType(BOOL_T);
    else if (m_isInt)
        setType(INT_T);
    else if (m_isFloat)
        setType(FLOAT_T);
    return os;
}


std::map<int, std::string> OtherExpression::m_func_to_string{{DP3, "dp3"}, {LIT, "lit"}, {RSQ, "rsq"}};

OtherExpression::OtherExpression(Expression* expression)
        :Expression(expression->isConstExpr()), m_expression(expression),
         m_variable(nullptr), m_arguments(nullptr), m_typeNode(nullptr), m_func(-1) {}

OtherExpression::OtherExpression(Variable* variable)
        :Expression(false), m_variable(variable),
         m_expression(nullptr), m_arguments(nullptr), m_typeNode(nullptr), m_func(-1) {}

OtherExpression::OtherExpression(TypeNode* _type, Arguments* arguments)
        :Expression(false), m_typeNode(_type), m_arguments(arguments),
         m_expression(nullptr), m_variable(nullptr), m_func(-1) {}

OtherExpression::OtherExpression(int func, Arguments* arguments)
        :Expression(false), m_func(func), m_arguments(arguments),
         m_expression(nullptr), m_variable(nullptr), m_typeNode(nullptr) {}

OtherExpression::~OtherExpression() {
    delete m_expression;
    delete m_variable;
    delete m_arguments;
    delete m_typeNode;
}

std::ostream& OtherExpression::write(std::ostream &os) const {
    if (m_expression)
        return os << "( " << m_expression << " )";
    else if (m_variable)
        return os << m_variable;
    else if (m_typeNode)
        return os << "(CALL " << m_typeNode << " " << m_arguments << ")";
    else
        return os << "(CALL " << m_func_to_string[m_func] << " " << m_arguments << ")";
}

std::ostream& OtherExpression::populateSymbolTableAndCheckErrors(std::ostream &os) {
    if (m_expression) m_expression->populateSymbolTableAndCheckErrors(os);
    if (m_variable) m_variable->populateSymbolTableAndCheckErrors(os);
    if (m_arguments) m_arguments->populateSymbolTableAndCheckErrors(os);
    return os;
}

std::ostream& OtherExpression::populateTypeAndCheckErrors(std::ostream &os) {
    if (m_expression) m_expression->populateTypeAndCheckErrors(os);
    if (m_variable) m_variable->populateTypeAndCheckErrors(os);
    if (m_arguments) m_arguments->populateTypeAndCheckErrors(os);

    if (m_arguments) //3-like operators
    {
        if (m_typeNode) { //type ( arguments_opt )
            if ((m_typeNode->type().baseType() != ANY_T) && (m_arguments->type().baseType() != ANY_T)) {
                if (m_arguments->type().vecSize() != 1) {
                    foundSemanticError();
                    os << semanticErrorHeader()
                       << "In constructor: " << m_typeNode << " (" << m_arguments << ") "
                       << std::endl << "Type of arguments must be basic types";
                    setType(ANY_T);
                }
                else if (m_arguments->type().baseType() != m_typeNode->type().baseType()) {
                    foundSemanticError();
                    os << semanticErrorHeader()
                       << "In constructor: " << m_typeNode << " (" << m_arguments << ") "
                       << std::endl << "Base type of '" << m_typeNode->type() << "' does not match '" << m_arguments->type() << "'";
                    setType(ANY_T);
                }
                else if (m_arguments->numArgs() != m_typeNode->type().vecSize()) {
                    foundSemanticError();
                    os << semanticErrorHeader()
                       << "In constructor: " << m_typeNode << " (" << m_arguments << ") "
                       << std::endl << "Number of expressions in arguments does not match size of type '"
                       << m_typeNode->type() << "'";
                    setType(ANY_T);
                }
            }
            else setType(ANY_T);
        }
        else { //FUNC ( arguments_opt )
            if (m_func == RSQ) {
                if (!((exactEqual(m_arguments->type(), Type(FLOAT_T)) || exactEqual(m_arguments->type(), Type(INT_T)))
                    && m_arguments->type().vecSize() == 1)) {
                    foundSemanticError();
                    os << semanticErrorHeader()
                       << "In function call: " << this
                       << std::endl << "Arguments don't match signature 'float rsq(float|int)'";
                    setType(ANY_T);
                }
                else setType(FLOAT_T);
            } else if (m_func == DP3) {
                if (!(exactEqual(m_arguments->type(), Type(VEC_T, 4)) || exactEqual(m_arguments->type(), Type(VEC_T, 3)) ||
                      exactEqual(m_arguments->type(), Type(IVEC_T, 4)) || exactEqual(m_arguments->type(), Type(IVEC_T, 3)))
                    && !(m_arguments->type().baseType() == ANY_T) && (m_arguments->type().vecSize() != 2)) {
                    foundSemanticError();
                    os << semanticErrorHeader()
                       << "In function call: " << this
                       << std::endl
                       << "Arguments don't match signature 'float dp3(vec4|3, vec4|3)' or 'int dp3(ivec4|3, ivec4|3";
                    setType(ANY_T);
                }
                else setType(Type(m_arguments->type().baseType()));
            } else if (m_func == LIT) {
                if (!exactEqual(m_arguments->type(), Type(VEC_T, 4))) {
                    foundSemanticError();
                    os << semanticErrorHeader()
                       << "In function call: " << this
                       << std::endl << "Arguments don't match signature 'vec4 lit(vec4)'";
                    setType(ANY_T);
                }
                else setType(Type(VEC_T, 4));
            }
        }
    }
    else if (m_expression)
        setType(m_expression->type());
    else if (m_variable)
        setType(m_variable->type());
    return os;
}


// Variable----------------------------------------
Variable::Variable(const std::string& _ID)
        :m_ID(_ID), m_index(0), m_isIndexPresent(false) {}

Variable::Variable(const std::string& _ID, int index)
        :m_ID(_ID), m_index(index), m_isIndexPresent(true) {}

std::ostream& Variable::write(std::ostream &os) const {
	if(m_isIndexPresent) os << "(INDEX " << type() << " ";
	os << m_ID << (m_isIndexPresent? (" " + std::to_string(m_index) + ")") : "");
    return os;
}

std::ostream& Variable::populateSymbolTableAndCheckErrors(std::ostream &os) {
    if (!m_symbolTable.findElementInStack(m_ID)) {
        foundSemanticError();
        os << semanticErrorHeader()
           << "Variable '" << this << "' used before being defined";
        //Push variable of
        m_symbolTable.pushElement(Symbol(false, m_ID, Type(ANY_T)));
    }
    return os;
}

std::ostream& Variable::populateTypeAndCheckErrors(std::ostream &os) {
    Symbol symbol = m_symbolTable.getElementInStack(m_ID); //Guaranteed to be there, at least with ANY_T
    if (m_isIndexPresent && !(0 <= m_index && m_index < symbol.type().vecSize())) {
        foundSemanticError();
        os << semanticErrorHeader()
           << "In variable: " << this
           << std::endl << "Access is out of bounds of the variable type '" << symbol.type() << "'";
        setType(ANY_T);
    }
    else setType(symbol.type());
    return os;
}


// Arguments---------------------------------------
Arguments::Arguments(Arguments* arguments, Expression* expression)
        :m_arguments(arguments), m_expression(expression), m_numExpressions(0) {
    if (m_expression)
        m_numExpressions += 1;
    if (m_arguments)
        m_numExpressions += m_arguments->m_numExpressions;
}

Arguments::~Arguments() {
    delete m_arguments;
    delete m_expression;
}

std::ostream& Arguments::write(std::ostream &os) const {
    return os << m_arguments << ((m_arguments && m_expression)? ", " : "") << m_expression;
}

bool Arguments::isConstExpr() const {
    return (m_expression? m_expression->isConstExpr() : true) &&
           (m_arguments? m_arguments->isConstExpr() : true);
}

std::ostream& Arguments::populateSymbolTableAndCheckErrors(std::ostream &os) {
    if (m_arguments) m_arguments->populateSymbolTableAndCheckErrors(os);
    if (m_expression) m_expression->populateSymbolTableAndCheckErrors(os);
    return os;
}

std::ostream& Arguments::populateTypeAndCheckErrors(std::ostream &os) {
    if (m_arguments) m_arguments->populateTypeAndCheckErrors(os);
    if (m_expression) m_expression->populateTypeAndCheckErrors(os);

    if (m_arguments && m_expression) { //arguments, expression
        if (!exactEqual(m_arguments->type(), m_expression->type())) {
            foundSemanticError();
            os << semanticErrorHeader()
               << "In arguments: " << m_arguments << ", " << m_expression
               << "Argument types '" << m_arguments->type() << "' and '" << m_expression->type() << "' don't match";
            setType(ANY_T);
        } else setType(m_expression->type());
    }
    else if (m_expression) setType(m_expression->type());
    else if (m_arguments) setType(m_arguments->type());
    else setType(ANY_T);
    return os;
}



bool exactEqual(Type a, Type b) {
    if (a.baseType() == ANY_T || b.baseType() == ANY_T)
        return true;
    else return (a.enumGivenType() == b.enumGivenType() &&
                 a.baseType() == b.baseType() &&
                 a.vecSize() == a.vecSize());
}

bool validUnary(int op, Type a) {
    if (op == '!') //Unary Logical, base type of b must be bool
        return a.baseType() == BOOL_T;
    else if (op == UMINUS) //Unary negative, base type must be arithmetic
        return isArithmetic(a.baseType());
    throw std::runtime_error("validUnary: Unknown unary operator: " + std::to_string(op));
}

bool operationEqual(Type a, int op, Type b) {
    if (a.baseType() == ANY_T || b.baseType() == ANY_T)
        return true;

    //Binary operations, operands must have same base type, and if both are vectors they must have the same size
    if (a.baseType() != b.baseType() ||
        (a.vecSize() > 1 && b.vecSize() > 1 && a.vecSize() != b.vecSize()))
        return false;

    //Binary arithmetic operations, both operands must have arithmetic base types
    if (op == '+' || op == '-' || op == '*' || op == '/' || op == '^') {
        if (!(isArithmetic(a.baseType()) && isArithmetic(b.baseType())))
            return false;

        if (op == '+' || op == '-') //Binary add/sub, operands must have the same arithmetic base type, and vec_size
            return isArithmetic(a.baseType()) && isArithmetic(b.baseType()) && a.vecSize() == b.vecSize();

        else if (op == '*') //Mult, no new restrictions
            return true;

        if (op == '/' || op == '^') //Operands must be scalars
            return a.vecSize() == 1 && b.vecSize() == 1;
    }
    else if (op == AND || op == OR) //Binary logical, both operands must have base type bool, and same vec_size
        return a.baseType() == BOOL_T && b.baseType() == BOOL_T && (a.vecSize() == b.vecSize());
    //Binary comparison, must have arithmetic type, and same vec_size
    else if (op == '<' || op == LEQ || op == '>' || op == GEQ || op == EQ || op == NEQ) {
        if (!(isArithmetic(a.baseType()) && isArithmetic(b.baseType())) ||
            (a.vecSize() != b.vecSize()))
            return false;

        //Must be scalar
        if (op == '<' || op == LEQ || op == '>' || op == GEQ)
            return a.vecSize() == 1 && b.vecSize() == 1;

        // No new restrictions
        if (op == EQ || op == NEQ)
            return true;
    }
    throw std::runtime_error("Unknown operator: " + std::to_string(op));
}

bool constructorEqual(Type var, Type args, int numExpressions)
{

}

bool isArithmetic(int type)
{
    return type == INT_T || type == FLOAT_T;
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
