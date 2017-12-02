//
// Created by ramon on 02/12/17.
//

#include "codegen.hpp"

int genCode(Node* ast)
{
    ARBInstructionSequence instr_sequence = ast->genCode();
    return 1;
}