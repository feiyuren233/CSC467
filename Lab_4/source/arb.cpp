//
// Created by ramon on 02/12/17.
//

#include "arb.hpp"

int ARBVar::m_uniqueIDCount = 0;

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
        :m_ID(), m_isLiteral(false), m_isTemp(false), m_tempID(-1) {}

ARBVar::ARBVar(float val)
        :m_ID(std::to_string(val)), m_isLiteral(true),
         m_isTemp(false), m_tempID(-1) {}

ARBVar::ARBVar(std::string declared_name, int index)
        :m_isLiteral(false), m_isTemp(false), m_tempID(-1)
{
    if (m_preDefinedID_to_ARBID.find(declared_name) != m_preDefinedID_to_ARBID.end()) {
        m_ID = m_preDefinedID_to_ARBID.at(declared_name);
    } else {
        m_ID = declared_name;
    }

    if (index != -1) {
        std::vector<std::string> index_to_element{"x", "y", "z", "w"};
        m_ID += ("." + index_to_element[index]);
    }
}

ARBVar ARBVar::makeTemp(int index)
{
    ARBVar temp;
    temp.m_ID = "tempVar" + std::to_string(m_uniqueIDCount++);
    temp.m_isTemp = true;
    temp.m_tempID = m_uniqueIDCount;
    return temp;
}


const std::map<ARBInstID, std::string> ARBInstruction::m_ARBInstId_to_string {
        {ARBInstID::ADD, "ADD"},
        {ARBInstID::CMP, "CMP"},
        {ARBInstID::MOV, "MOV"},
        {ARBInstID::TEMP, "TEMP"},
        {ARBInstID::MUL, "MUL"}
        //TODO: complete this map
};

ARBVar ARBInstruction::changeResultVar(ARBVar _new_result) {
    ARBVar temp = m_resultVar;
    m_resultVar = _new_result;
    return temp;
}

std::string ARBInstruction::to_string() {
    std::string inst = m_ARBInstId_to_string.at(m_ID) + " " + m_resultVar.id();
    for (auto var : m_argumentVars)
        inst += (", " + var.id());
    inst += ";";
    if (!m_comment.empty())
        inst += (" # " + m_comment);
    return inst;
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