
#include "semantic.hpp"
#include <iostream>

int semantic_check(Node *ast) {
    ast->populateSymbolTableAndCheckErrors(std::cout);
    return ast->semanticallyCorrect()? 0 : 1;
}
