//
// Created by ramon on 02/12/17.
//

#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.hpp"
#include "symbol.hpp"
#include "arb.hpp"

int genCode(Node* ast);

ARBInstructionSequence genTempVariableDeclarations(ARBInstructionSequence& sequence);

//AST genCode definitions --------------------------------------------------
ARBInstructionSequence concatenateCode(Node* a, Node* b);

#endif //CODEGEN_H
