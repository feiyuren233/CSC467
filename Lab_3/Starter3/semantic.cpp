
#include "semantic.hpp"
#include <iostream>

int semantic_check(Node *ast) {
    ast->populateSymbolTableAndCheckErrors(std::cout);
    ast->populateTypeAndCheckErrors(std::cout);
    return ast->semanticallyCorrect()? 0 : 1;
}
