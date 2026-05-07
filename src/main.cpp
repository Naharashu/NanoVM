#include <cmath>
#include <iostream>
#include "vm.h"



int main(int argc, char** argv) {

    NanoVM vm{};

    int res = vm.res();
    std::cout << res << '\n';

    return 0;
}