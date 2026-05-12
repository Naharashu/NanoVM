#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>
#include "vm.h"



int main(int argc, char** argv) {

    NanoVM vm{};
    std::vector<uint8_t> program = {
        JMP, 11,
        LD, 0,1,0,0,0,0,0,0,0,
        LD, 1, 2,0,0,0,0,0,0,0,
        ADD, 0, 1,
        HLT
    };
    vm.load_program(program);
    vm.run(0);
    int res = vm.res();
    std::cout << vm.reg[0] << '\n';

    return 0;
}