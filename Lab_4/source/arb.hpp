//
// Created by ramon on 02/12/17.
//

#ifndef ARB_INSTRUCTIONS_H
#define ARB_INSTRUCTIONS_H

#include <string>
#include <map>
#include "symbol.hpp"

class ARBVar
{
public:
    ARBVar(); //Temp variable
    ARBVar(float val); //Literal
    ARBVar(std::string declared_name); //Declared variable

    std::string id() { return m_ID; }
    bool empty() { return m_ID.empty(); }
    bool isLiteral() { return m_isLiteral; }

private:
    std::string m_ID;
    bool m_isLiteral;

    static int m_uniqueIDCount;
    static const std::map<std::string, std::string> m_preDefinedID_to_ARBID;
};
typedef std::vector<ARBVar> ARBVars;

enum class ARBInstID {
    ABS, ADD, CMP, COS, DP3, DP4, DPH, DST, EX2, FLR, FRC, KIL,
    LG2, LIT, LRP, MAD, MAX, MIN, MOV, MUL, POW, RCP, RSQ, SCS,
    SGE, SIN, SLT, SUB, SWZ, TEX, TXB, TXP, XPD, TEMP,
    UNKNOWN //debugging
};

class ARBInstruction
{
public:
    ARBInstruction(ARBInstID _id, ARBVar _result, ARBVars _arguments, std::string comment = "")
            :m_ID(_id), m_resultVar(_result), m_argumentVars(_arguments), m_comment(comment) {}

    ARBInstID id() { return m_ID; }
    ARBVar resultVar() { return m_resultVar; }
    ARBVars argVars() { return m_argumentVars; }

    ARBVar changeResultVar(ARBVar _new_result);

    std::string to_string();

private:
    ARBInstID m_ID;
    ARBVar m_resultVar;
    std::vector<ARBVar> m_argumentVars;
    std::string m_comment;

    static const std::map<ARBInstID, std::string> m_ARBInstId_to_string;
};
typedef std::vector<ARBInstruction> ARBInstructions;


class ARBInstructionSequence
{
public:
    ARBInstructions& instructions() { return m_instructions; }
    ARBInstructionSequence& push(const ARBInstruction& instruction);
    ARBInstructionSequence& push(const ARBInstructions& instructions);
    ARBInstructionSequence& push(const ARBInstructionSequence& instruction_sequence);

    void setResultVar(ARBVar _result) { m_resultVar = _result; }
    ARBVar resultVar() { return m_resultVar; }

private:
    ARBVar m_resultVar;
    std::vector<ARBInstruction> m_instructions;
};

#endif //ARB_INSTRUCTIONS_H
