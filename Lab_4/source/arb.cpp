//
// Created by ramon on 02/12/17.
//

#include "arb.hpp"

int ARBVar::m_uniqueID = 0;

const std::map<std::string, std::string> ARBVar::m_preDefinedID_to_ARBID {
        {"gl_FragColor", "result.color"},
        {"gl_FragDepth", "result.depth"},
        {"gl_FragCoord", "fragment.position"},
        {"gl_TexCoord", "fragment.texcoord"},
        {"gl_Color", "fragment.color"},
        {"gl_Secondary", "fragment.color.secondary"},
        {"gl_FogFragCoord", "fragment.fogcoord"},
        {"gl_Light_Half", "state.light[0].half"},
        {"gl_Light_Ambient", "state.lightmodel.ambient"},
        {"gl_Material_Shininess", "state.material.shininess"},
        {"env1", "program.env[1]"},
        {"env2", "program.env[2]"},
        {"env3", "program.env[3]"}
};

ARBVar::ARBVar()
        :m_ID("temp_var_" + std::to_string(m_uniqueID++)), m_isLiteral(false) {}

ARBVar::ARBVar(float val)
        :m_ID(std::to_string(val)), m_isLiteral(true) {}

ARBVar::ARBVar(std::string declared_name) :m_isLiteral(false)
{
    if (m_preDefinedID_to_ARBID.find(declared_name) != m_preDefinedID_to_ARBID.end()) {
        m_ID = m_preDefinedID_to_ARBID.at(declared_name);
    } else {
        m_ID = m_uniqueID;
    }
}


ARBVar ARBInstruction::changeResultVar(ARBVar _new_result) {
    ARBVar temp = m_resultVar;
    m_resultVar = _new_result;
    return temp;
}


ARBInstructionSequence& ARBInstructionSequence::push(const ARBInstruction &instruction) {
    m_instructions.push_back(instruction);
    return *this;
}

ARBInstructionSequence& ARBInstructionSequence::push(const ARBInstructions &instructions) {
    m_instructions.insert(m_instructions.end(), instructions.begin(), instructions.end());
    return *this;
}

ARBInstructionSequence& ARBInstructionSequence::push(const ARBInstructionSequence &instruction_sequence) {
    push(instruction_sequence.m_instructions);
    return *this;
}