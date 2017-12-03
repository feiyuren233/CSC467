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
        :m_ID("temp_var_" + std::to_string(m_uniqueID++)) {}

ARBVar::ARBVar(std::string pre_defined_id)
        :m_ID(m_preDefinedID_to_ARBID.at(pre_defined_id)) {}

ARBInstructionSequence& ARBInstructionSequence::push(ARBInstruction &instruction) {
    m_instructions.push_back(instruction);
    return *this;
}

ARBInstructionSequence& ARBInstructionSequence::push(ARBInstructions &instructions) {
    m_instructions.insert(m_instructions.end(), instructions.begin(), instructions.end());
    return *this;
}

ARBInstructionSequence& ARBInstructionSequence::push(ARBInstructionSequence &instruction_sequence) {
    push(instruction_sequence.m_instructions);
    return *this;
}