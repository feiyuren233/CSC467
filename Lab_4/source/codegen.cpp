//
// Created by ramon on 02/12/17.
//

#include "codegen.hpp"
#include "common.h"
#include "parser.h"
#include <sstream>

int genCode(Node* ast)
{
    ARBInstructionSequence instr_sequence = ast->genCode();
    ARBInstructionSequence declaration_sequence = genTempVariableDeclarations(instr_sequence);

    std::stringstream output_ss;
    output_ss << "!!ARBfp1.0" << std::endl;

    for (auto instruction : declaration_sequence.push(instr_sequence).instructions())
         output_ss << instruction.to_string() << std::endl;

    output_ss << "END" << std::endl;

    fprintf(outputFile, output_ss.str().c_str());
    return 0;
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
        ARBInstructionSequence condition_expression_sequence = m_expression->genCode();
        ARBVar condition = condition_expression_sequence.resultVar();
        ARBInstructionSequence if_sequence = m_statement->genCode();
        ARBInstructionSequence else_sequence = m_elseStatement? m_elseStatement->genCode() : ARBInstructionSequence();

        sequence.push(condition_expression_sequence);
        sequence.push(ARBInstruction(
                ARBInstID::MUL,
                condition,
                ARBVars{ARBVar(-1), condition},
                "CONDITION"
        ));

        ARBVar temp = ARBVar::makeTemp();
        for (auto instruction : if_sequence.instructions()) {
            ARBVar result = instruction.changeResultVar(temp);
            sequence.push(instruction);
            sequence.push(ARBInstruction(
                    ARBInstID::CMP,
                    result,
                    ARBVars{condition, temp, result},
                    "IF"
            ));
        }

        for (auto instruction : else_sequence.instructions()) {
            ARBVar result = instruction.changeResultVar(temp);
            sequence.push(instruction);
            sequence.push(ARBInstruction(
                    ARBInstID::CMP,
                    result,
                    ARBVars{condition, result, temp},
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
    ARBVar result2 = ARBVar::makeTemp();
	ARBVar onep1 = ARBVar(1.1);
	ARBVar logic0 = ARBVar(0);
	ARBVar logic1 = ARBVar(1);
    sequence.setResultVar(result);

    if (m_lhs && m_rhs) { //Binary operation
        //TODO: implement!
		switch(m_operator){
			case '+': sequence.push(ARBInstruction(
                ARBInstID::ADD,
                result,
                ARBVars{lhs_expression_sequence.resultVar(), rhs_expression_sequence.resultVar()},
                "EXPRESSION (binary +)")); break;
			case '-': sequence.push(ARBInstruction(
                ARBInstID::SUB,
                result,
                ARBVars{lhs_expression_sequence.resultVar(), rhs_expression_sequence.resultVar()},
                "EXPRESSION (binary -)")); break;;
			case '*': sequence.push(ARBInstruction(
                ARBInstID::MUL,
                result,
                ARBVars{lhs_expression_sequence.resultVar(), rhs_expression_sequence.resultVar()},
                "EXPRESSION (binary *)")); break;
			case '/': sequence.push(ARBInstruction(
                ARBInstID::RCP,
                result,
                ARBVars{rhs_expression_sequence.resultVar()}));//rhs^-1
				
				sequence.push(ARBInstruction(
                ARBInstID::MUL,
                result2,
                ARBVars{lhs_expression_sequence.resultVar(), result},
                "EXPRESSION (binary /)")); break;
			case '^': sequence.push(ARBInstruction(
                ARBInstID::POW,
                result,
                ARBVars{lhs_expression_sequence.resultVar(), rhs_expression_sequence.resultVar()},
                "EXPRESSION (binary ^)")); break;
			case '<': sequence.push(ARBInstruction(
                ARBInstID::SLT,
                result,
                ARBVars{lhs_expression_sequence.resultVar(), rhs_expression_sequence.resultVar()},
                "EXPRESSION (binary <)")); break; 
			case LEQ: sequence.push(ARBInstruction(
                ARBInstID::SGE,
                result,
                ARBVars{rhs_expression_sequence.resultVar(), lhs_expression_sequence.resultVar()},
                "EXPRESSION (binary <=)")); break;			
			case '>': sequence.push(ARBInstruction(
                ARBInstID::SLT,
                result,
                ARBVars{rhs_expression_sequence.resultVar(), lhs_expression_sequence.resultVar()},
                "EXPRESSION (binary >)")); break; 
			case GEQ: sequence.push(ARBInstruction(
                ARBInstID::SGE,
                result,
                ARBVars{lhs_expression_sequence.resultVar(), rhs_expression_sequence.resultVar()},
                "EXPRESSION (binary >=)")); break;
			case AND: sequence.push(ARBInstruction(
                ARBInstID::ADD,
                result,
                ARBVars{lhs_expression_sequence.resultVar(), rhs_expression_sequence.resultVar()}));

				sequence.push(ARBInstruction(
                ARBInstID::SUB,
                result2,
                ARBVars{result, onep1}));

				sequence.push(ARBInstruction(
                ARBInstID::CMP,
                result,
                ARBVars{result2, logic0, logic1},
                "EXPRESSION (binary &&)")); break;
			case OR: sequence.push(ARBInstruction(
                ARBInstID::ADD,
                result,
                ARBVars{lhs_expression_sequence.resultVar(), rhs_expression_sequence.resultVar()}));

				sequence.push(ARBInstruction(
                ARBInstID::SUB,
                result2,
                ARBVars{result, logic1}));

				sequence.push(ARBInstruction(
                ARBInstID::CMP,
                result,
                ARBVars{result2, logic0, logic1},
                "EXPRESSION (binary ||)")); break;
			case EQ: sequence.push(ARBInstruction(
                ARBInstID::SGE,
                result,
                ARBVars{lhs_expression_sequence.resultVar(), rhs_expression_sequence.resultVar()}));

				sequence.push(ARBInstruction(
                ARBInstID::SGE,
                result2,
                ARBVars{rhs_expression_sequence.resultVar(), lhs_expression_sequence.resultVar()}));

				sequence.push(ARBInstruction(
                ARBInstID::ADD,
                result,
                ARBVars{result, result2}));

				sequence.push(ARBInstruction(
                ARBInstID::SUB, 
                result2,
                ARBVars{result, onep1}));

				sequence.push(ARBInstruction(
                ARBInstID::CMP,
                result,
                ARBVars{result2, logic0, logic1},
                "EXPRESSION (binary ==)")); break;
			case NEQ: sequence.push(ARBInstruction(
                ARBInstID::SGE,
                result,
                ARBVars{lhs_expression_sequence.resultVar(), rhs_expression_sequence.resultVar()}));

				sequence.push(ARBInstruction(
                ARBInstID::SGE,
                result2,
                ARBVars{rhs_expression_sequence.resultVar(), lhs_expression_sequence.resultVar()}));

				sequence.push(ARBInstruction(
                ARBInstID::ADD,
                result,
                ARBVars{result, result2}));

				sequence.push(ARBInstruction(
                ARBInstID::SUB, 
                result2,
                ARBVars{result, onep1}));

				sequence.push(ARBInstruction(
                ARBInstID::CMP,
                result,
                ARBVars{result2, logic1, logic0},
                "EXPRESSION (binary !=)")); break;			
		}
		
        
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

    ARBVar arg;
    if (m_isBool)
        arg = ARBVar(m_valBool? 1 : 0);
    else if (m_isInt)
        arg = ARBVar(float(m_valInt));
    else if (m_isFloat)
        arg = ARBVar(m_valFloat);

    sequence.push(ARBInstruction(
            ARBInstID::MOV,
            result,
            ARBVars{arg},
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
        if (m_typeNode) { //type (arguments_opt)
            //TODO: implement!
            ARBVar result = ARBVar::makeTemp();
            std::vector<Expression*> expressions = m_arguments->generateExpressionVec();

            for (int i = 0; i < expressions.size(); i++) {
                ARBInstructionSequence expression_sequence = expressions[i]->genCode();
                sequence.push(expression_sequence);

                sequence.push(ARBInstruction(
                        ARBInstID::MOV,
                        ARBVar(result.id(), i),
                        ARBVars{expression_sequence.resultVar()}
                ));
            }
        } else
        { //function call
            ARBVar result = ARBVar::makeTemp();
            std::vector<Expression*> expressions = m_arguments->generateExpressionVec();
            ARBVars arguments;

            for (auto expression : expressions ) {
                ARBInstructionSequence expression_sequence = expression->genCode();
                sequence.push(expression_sequence);
                arguments.push_back(expression_sequence.resultVar());
            }

            sequence.push(ARBInstruction(
                    m_func_to_ARBInstID.at(m_func),
                    result,
                    arguments,
                    "FUNCTION CALL"
            ));
        }
    }
    return sequence;
}
