#ifndef NANOVM_H
#define NANOVM_H

#include <asmjit/core/codeholder.h>
#include <asmjit/core/jitruntime.h>
#include <asmjit/core/operand.h>
#include <asmjit/x86/x86assembler.h>
#include <asmjit/x86/x86operand.h>
#include <asmjit/x86.h>
#include <cstdint>
#include <array>
#include <iostream>
#include <vector>

#define MEM_SIZE 65536
#define FETCH (this->memory[this->pc++])

#define NUL 0x0
#define LD 0x01
#define ADD 0x02 
#define SUB 0x03
#define MUL 0x04
#define DIV 0x05
#define HLT 0xFF

using namespace asmjit;


class NanoVM {
    public:
    typedef int (*Func)(void* mem);

    JitRuntime rt;
    CodeHolder code;

    uint8_t memory[MEM_SIZE];
    uint64_t reg[256];
    std::array<bool, 8> flags;
    uint64_t pc=0;
    uint64_t prog_size = 0;

    NanoVM() {
        code.init(rt.environment(), rt.cpu_features());
        for(uint64_t &x : reg) x = 0; 
        for(uint8_t &y : memory) y = 0;
    }


    uint64_t fetch64(uint64_t i) {
        uint64_t res = 0;
        for(int a=0;a<8;a++) {
            res |= (uint64_t)(this->memory[i++]) << (8*a);
        }
        return res;
    }


    void run(int32_t ip) {
        int i = ip;
        x86::Assembler a(&code);
        
        while(i<prog_size) {
            i=FETCH;
            switch(i) {
                case NUL: continue; break;
                case LD: {
                    uint8_t r = FETCH;
                    uint64_t val = fetch64(pc);
                    pc+=8;
                    a.mov(x86::regs::rax, val);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r * 8), x86::regs::rax);
                    break;
                }
                case ADD: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.add(x86::regs::rax, x86::regs::rcx);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    break;
                }
                case SUB: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.sub(x86::regs::rax, x86::regs::rcx);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    break;
                }
                case MUL: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.mul(x86::regs::rcx);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    break;
                }
                case DIV: {
                    uint8_t r = FETCH;
                    uint8_t r2 = FETCH;
                    a.mov(x86::regs::rcx, x86::qword_ptr(x86::regs::rdi, r2*8));
                    a.cmp(x86::regs::rcx, 0);
                    Label skip_div = a.new_label();
                    a.jz(skip_div);
                    a.xor_(x86::regs::rdx, x86::regs::rdx);
                    a.mov(x86::regs::rax, x86::qword_ptr(x86::regs::rdi, r*8));
                    a.div(x86::regs::rcx);
                    a.mov(x86::qword_ptr(x86::regs::rdi, r*8), x86::regs::rax);
                    a.bind(skip_div);
                    break;
                }
                case HLT: {
                    a.xor_(x86::regs::rax, x86::regs::rax);
                    a.ret();
                }
            }

        }
    }

    int res() {
        Func fn;
        Error err = rt.add(&fn, &code);
        int res = fn(this->reg);
        rt.release(fn);
        return res;
    }

    void load_program(const std::vector<uint8_t> &prog) {
        for (uint64_t i = 0; i < prog.size(); i++) {
            if (i >= MEM_SIZE) return;
            memory[i] = prog[i];
        }
        prog_size = prog.size();
    }

    ~NanoVM() {
        
    }
};

#endif