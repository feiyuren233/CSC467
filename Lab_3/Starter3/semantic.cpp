
#include "semantic.hpp"
#include <string.h>
#include <iostream>
#include <sstream>
#include "common.h"

int semantic_check(Node *ast) {
    std::stringstream os_ss;
    ast->populateSymbolTableAndCheckErrors(os_ss);
    ast->populateTypeAndCheckErrors(os_ss);
    fprintf(errorFile, os_ss.str().c_str());
    return ast->semanticallyCorrect()? 0 : 1;
}
