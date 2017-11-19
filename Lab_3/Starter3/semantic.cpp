
#include "semantic.hpp"
#include <iostream>

int semantic_check(Node *ast) {
    ast->populateSymbolTableAndCheckDeclarationErrors(std::cout);
    return 1;
}
