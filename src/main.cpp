

#include <cstdint>
#include <cstring>
#include <vector>
#include "vm.h"



int main(int argc, char** argv) {

    NanoVM vm{};
    bool regdump = false;
    for(int i=0;i<argc;i++) {
        if(strcmp(argv[i], "-r")==0) regdump=true;
        if(strcmp(argv[i], "-V")==0) vm.verbose=true;
    }
    std::vector<uint8_t> program = {
        JMP, 20, 0,0,0,0,0,0,0,
        LD, 0, 1,0,0,0,0,0,0, 0,
        RET,
        CALL, 9, 0,0,0,0,0,0,0,
        LD, 1, 3, 0, 0, 0, 0, 0, 0, 0,
        FADD, 0, 1,
        HLT
    };
    vm.load_program(program);
    vm.run(0);
    int res = vm.res();
    std::cout << "Exit code " << res << '\n';
    if(regdump) {
        vm.register_dump();
    }


    return 0;
}