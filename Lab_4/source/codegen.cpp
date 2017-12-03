//
// Created by ramon on 02/12/17.
//

#include "codegen.hpp"

int genCode(Node* ast)
{
    ARBInstructionSequence instr_sequence = ast->genCode();
    ARBInstructionSequence declaration_sequence = genTempVariableDeclarations(instr_sequence);

    //TODO: Implement printing to file
    for (auto instruction : declaration_sequence.push(instr_sequence).instructions())
        std::cout << instruction.to_string() << std::endl;
    return 1;
}

ARBInstructionSequence genTempVariableDeclarations(ARBInstructionSequence& sequence)
{
    std::map<int, ARBVar> temp_vars;
    for (auto instruction : sequence.instructions())
    {
        if (instruction.resultVar().isTemp())
            temp_vars[instruction.resultVar().tempID()] = instruction.resultVar();

        for (auto var : instruction.argVars()) {
            if (var.isTemp())
                temp_vars[var.tempID()] = var;
        }
    }

    ARBInstructionSequence declaration_sequence;
    for (auto var : temp_vars)
        declaration_sequence.push(ARBInstruction(
               ARBInstID::TEMP,
               var.second,
               ARBVars()
        ));
    return declaration_sequence;
}

ARBInstructionSequence concatenateCode(Node *a, Node *b)
{
    ARBInstructionSequence temp = a? a->genCode() : ARBInstructionSequence();
    if (b)
        temp.push(b->genCode());
    return temp;
}

//AST genCode definitions -------------------------------------------
ARBInstructionSequence Scope::genCode() {
    enterScope(this);
        ARBInstructionSequence sequence = concatenateCode(m_declarations, m_statements);
    exitScope();
    return sequence;
}

ARBInstructionSequence Declarations::genCode() {
    return concatenateCode(m_declarations, m_declaration);
}

ARBInstructionSequence Statements::genCode() {
    return concatenateCode(m_statements, m_statement);
}

ARBInstructionSequence Declaration::genCode() {
    ARBInstructionSequence sequence;
    ARBVar declared_var = IDToARBVar(m_ID);
    ARBInstructionSequence expression_result_seq = m_expression? m_expression->genCode() : ARBInstructionSequence();

    sequence.push(expression_result_seq); //Expression has to be before declaration so that we can use its result
    sequence.push(ARBInstruction(
            ARBInstID::TEMP,
            declared_var,
            ARBVars(),
            "DECLARATION"
    ));

    if (!expression_result_seq.empty())
        sequence.push(ARBInstruction(
                ARBInstID::MOV,
                declared_var,
                ARBVars{expression_result_seq.resultVar()},
                "CONSTRUCTION"
        ));

    return sequence;
}

ARBInstructionSequence Statement::genCode() {
    if (!(m_scope || m_variable || m_expression || m_statement || m_elseStatement))
        return ARBInstructionSequence();
    else if (m_scope)
        return m_scope->genCode();
    else if (m_variable && m_expression) { //variable = expression
        ARBInstructionSequence sequence = m_expression->genCode();
        ARBVar variable = IDToARBVar(m_variable->id(), m_variable->hasIndex()? m_variable->index() : -1);

        if (!sequence.instructions().empty()) {
            sequence.instructions().back().changeResultVar(variable);
            sequence.setResultVar(variable);
        }
        else {
            sequence.push(ARBInstruction(
                    ARBInstID::MOV,
                    variable,
                    ARBVars{sequence.resultVar()},
                    "ASSIGNMENT"
            ));
            sequence.setResultVar(variable);
        }
        return sequence;
    }
    else if (m_expression && m_statement) { //if statement
        ARBInstructionSequence sequence;
        ARBInstructionSequence expression_sequence = m_expression->genCode();
        ARBVar condition = expression_sequence.resultVar();
        ARBInstructionSequence if_sequence = m_statement->genCode();
        ARBInstructionSequence else_sequence = m_elseStatement? m_elseStatement->genCode() : ARBInstructionSequence();

        sequence.push(expression_sequence);

        ARBVar temp = ARBVar::makeTemp();
        for (auto instruction : if_sequence.instructions()) {
            ARBVar result = instruction.changeResultVar(temp);
            sequence.push(instruction);
            sequence.push(ARBInstruction(
                    ARBInstID::CMP,
                    result,
                    ARBVars{condition, result, temp},
                    "IF"
            ));
        }

        for (auto instruction : else_sequence.instructions()) {
            ARBVar result = instruction.changeResultVar(temp);
            sequence.push(instruction);
            sequence.push(ARBInstruction(
                    ARBInstID::CMP,
                    result,
                    ARBVars{condition, temp, result},
                    "ELSE"
            ));
        };

        return sequence;
    } else throw std::runtime_error("Statement::genScope: No correct sub-type of Statement found");
}

ARBInstructionSequence OperationExpression::genCode() {
    ARBInstructionSequence sequence;
    ARBInstructionSequence lhs_expression_sequence = m_lhs? m_lhs->genCode() : ARBInstructionSequence();
    ARBInstructionSequence rhs_expression_sequence = m_rhs? m_rhs->genCode() : ARBInstructionSequence();
    sequence.push(lhs_expression_sequence);
    sequence.push(rhs_expression_sequence);

    ARBVar result = ARBVar::makeTemp();
    sequence.setResultVar(result);

    if (m_lhs && m_rhs) { //Binary operation
        //TODO: implement!
        sequence.push(ARBInstruction(
                ARBInstID::MUL,
                result,
                ARBVars{lhs_expression_sequence.resultVar(), rhs_expression_sequence.resultVar()},
                "EXPRESSION (binary)"
        ));
    } else { //Unary operation
        //TODO: implement!
        sequence.push(ARBInstruction(
                ARBInstID::MUL,
                result,
                ARBVars{rhs_expression_sequence.resultVar(), rhs_expression_sequence.resultVar()},
                "EXPRESSION (unary)"
        ));
    }

    return sequence;
}

ARBInstructionSequence LiteralExpression::genCode() {
    ARBInstructionSequence sequence;

    ARBVar result = ARBVar::makeTemp();
    sequence.setResultVar(result);

    sequence.push(ARBInstruction(
            ARBInstID::MOV,
            result,
            ARBVars{ARBVar(m_valFloat)},
            "LITERAL"
    ));
    return sequence;
}

ARBInstructionSequence OtherExpression::genCode() {
    ARBInstructionSequence sequence;
    if (m_expression) { //(expression)
        ARBInstructionSequence expression_sequence = m_expression->genCode();
        sequence.push(expression_sequence);
        sequence.setResultVar(expression_sequence.resultVar());
    }
    else if (m_variable) { //variable
        sequence.setResultVar(IDToARBVar(m_variable->id(), m_variable->hasIndex()? m_variable->index() : -1));
    }
    else if (m_arguments) { //function call or constructor
        //TODO: implement!
    }
    return sequence;
}