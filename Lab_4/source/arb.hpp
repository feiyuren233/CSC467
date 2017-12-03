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
    ARBVar();
    ARBVar(std::string pre_defined_id);
    std::string id() { return m_ID; }
    bool empty() { return m_ID.empty(); }

private:
    std::string m_ID;
    static int m_uniqueID;
    static const std::map<std::string, std::string> m_preDefinedID_to_ARBID;
};
typedef std::vector<ARBVar> ARBVars;

enum class ARBInstID {
    ABS, ADD, CMP, COS, DP3, DP4, DPH, DST, EX2, FLR, FRC, KIL,
    LG2, LIT, LRP, MAD, MAX, MIN, MOV, MUL, POW, RCP, RSQ, SCS,
    SGE, SIN, SLT, SUB, SWZ, TEX, TXB, TXP, XPD,
    UNKNOWN //debugging
};

class ARBInstruction
{
public:
    ARBInstruction(ARBInstID _id, ARBVar _result, ARBVars _arguments)
            :m_ID(_id), m_resultVar(_result), m_argumentVars(_arguments) {}

    ARBInstID id() { return m_ID; }
    ARBVar resultVar() { return m_resultVar; }
    ARBVars argVars() { return m_argumentVars; }

private:
    ARBInstID m_ID;
    ARBVar m_resultVar;
    std::vector<ARBVar> m_argumentVars;
};
typedef std::vector<ARBInstruction> ARBInstructions;


class ARBInstructionSequence
{
public:
    const ARBInstructions& instructions() { return m_instructions; }
    ARBInstructionSequence& push(ARBInstruction& instruction);
    ARBInstructionSequence& push(ARBInstructions& instructions);
    ARBInstructionSequence& push(ARBInstructionSequence& instruction_sequence);

private:
    std::vector<ARBInstruction> m_instructions;
};

#endif //ARB_INSTRUCTIONS_H
