#include <iostream>
#include <fstream>
#include "commands.hpp"

int main() {
    ifstream in("all.in");
    while (1) read_command(std::cin, std::cout);
    return 0;
}