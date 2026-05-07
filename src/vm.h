#ifndef NANOVM_H
#define NANOVM_H

#include <asmjit/core/codeholder.h>
#include <asmjit/core/jitruntime.h>
#include <asmjit/x86/x86assembler.h>
#include <asmjit/x86/x86operand.h>
#include <asmjit/x86.h>
#include <cstdint>
#include <array>

using namespace asmjit;


class NanoVM {
    public:
    typedef int (*Func)(void);

    JitRuntime rt;
    CodeHolder code;

    std::array<uint8_t, 65536> memory;
    std::array<uint64_t, 15> reg;
    std::array<bool, 8> flags;

    NanoVM() {
        code.init(rt.environment(), rt.cpu_features());
    }



    int res() {
        Func fn;
        Error err = rt.add(&fn, &code);
        int res = fn();
        rt.release(fn);
        return res;
    }

    ~NanoVM() {
        
    }
};

#endif