#ifndef NANOVM_ASSEMBLY_H
#define NANOVM_ASSEMBLY_H

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <array>
#include "../common.h"

class assembly_error : public std::exception {
    public:
    std::string err="";
    explicit assembly_error(const std::string &s) {
        err = s;
    }
    const char * what() const noexcept override {
        return err.c_str();
    }
};

enum tok_type : uint8_t
{
    ID,
    COMA,
    SEMI,
    NEWLINE,
    INT,
    SHORT_INT,
    WORD,
    LONG_INT,
    REGN,
    LABEL,
    INCLUDE,
    EOF_
};

typedef struct token
{
    tok_type t;
    uint64_t line;
    uint16_t c;
    std::string val = "";
    uint64_t address=0;
} token;

std::unordered_map<std::string, uint64_t> labels;

class lexer
{
public:
    std::string code = "";
    std::vector<token> lexed;
    std::string filename = "";
    std::string header;
    explicit lexer(const std::string &c) {
        std::string preproccesored = preproccessor(c);
        code += header;
        code += preproccesored;
    };

    inline bool is_int(unsigned char c)
    {
        return c >= '0' && c <= '9';
    }

    inline bool is_letter(unsigned char c)
    {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }

    inline bool is_opcode(const std::string &id) {
        bool opcode = false;
        if(id=="ld") opcode = true;
        if(id=="add") opcode = true;
        if(id=="sub") opcode = true;
        if(id=="mul") opcode = true;
        if(id=="div") opcode = true;
        if(id=="imul") opcode = true;
        if(id=="idiv") opcode = true;
        if(id=="xor") opcode = true;
        if(id=="and") opcode = true;
        if(id=="or") opcode = true;
        if(id=="shl") opcode = true;
        if(id=="shr") opcode = true;
        if(id=="jmp") opcode = true;
        if(id=="cmp") opcode = true;
        if(id=="jz") opcode = true;
        if(id=="jnz") opcode = true;
        if(id=="jc") opcode = true;
        if(id=="jnc") opcode = true;
        if(id=="store") opcode = true;
        if(id=="ldm") opcode = true;
        if(id=="jl") opcode = true;
        if(id=="jle") opcode = true;
        if(id=="jb") opcode = true;
        if(id=="jbe") opcode = true;
        if(id=="jmprv") opcode = true;
        if(id=="push") opcode = true;
        if(id=="pop") opcode = true;
        if(id=="call") opcode = true;
        if(id=="ret") opcode = true;
        if(id=="fadd") opcode = true;
        if(id=="fsub") opcode = true;
        if(id=="fmul") opcode = true;
        if(id=="fdiv") opcode = true;
        /*
        if(id=="ADD") opcode = true;
        if(id=="ADD") opcode = true;
        if(id=="ADD") opcode = true;
        if(id=="ADD") opcode = true;
        if(id=="ADD") opcode = true;
        */
        if(id=="hlt") opcode = true;
        return opcode;
    }

    std::string preproccessor(std::string code) {
        uint64_t l=0;
        uint64_t c=0;
        for(uint64_t i=0; i < code.size();) {
            uint8_t c = code[i];
            if(c=='#') {
                std::string id;
                code[i]=' ';
                i++; // skip '#'
                while (i < code.size() && (is_letter(code[i])))
                {
                    id.push_back(code[i]);
                    code[i]=' ';
                    i++;
                    c++;
                }
                if (id != "include" || code[i] != ' ') {
                    throw assembly_error("[Error - assembly:" + filename + std::to_string(l) + ':' + std::to_string(c) + "]: expected #include, got '" + id + "'\n");
                }
                if(code[i]!=' ') throw assembly_error("[Error - assembly:" + filename + std::to_string(l) + ':' + std::to_string(c) + "]: expected space\n");
                i++; // skip ' '
                std::string fname;
                while (i < code.size() && (is_letter(code[i])||code[i]=='_'||code[i]=='.'|code[i]=='/'))
                {
                    fname.push_back(code[i]);
                    code[i]=' ';
                    i++;
                    c++;
                }
                std::ifstream f(fname);
                if(!f.is_open()) throw assembly_error("[Error - preproccesor]: file '" + fname + "' doesnt exitst\n");
                std::string line;
                while(std::getline(f, line)) {
                    header += line + '\n';
                }
            }
            else if(i<code.size()&&code[i]=='\n') {
                i++;
                l++;
            }
            else {
                i++;
                c++;
            }
        }
        return code;
    }

    void collect_labels() {
        uint64_t address = 0;
        for(uint64_t i=0; i < code.size();) {
            uint8_t c = code[i];
            if(is_letter(c)||c=='_') {
                std::string id;
                while (i < code.size() && (is_letter(code[i]) || is_int(code[i]) || code[i] == '_' || code[i] == ':'))
                {
                    id.push_back(code[i]);
                    i++;
                }
                if(!id.empty() && id.back() == ':') {
                    id.pop_back();
                    labels[id] = address;
                } else if(is_opcode(id)) {
                    address++;
                } else if((id[0]=='R'||id[0]=='r')&&std::all_of(id.begin()+1, id.end(),::isdigit)) {
                    address++;
                } else address+=8;
            } else if(is_int(c)) {
                while ((i < code.size() && is_int(code[i])))
                {
                    i++;
                }
                address+=8;
            } else if (c == ';') {
                i++;
                while (i < code.size()&&code[i] != '\n')
                    i++;
            } else i++;
        }
    }

    void lex()
    {
        uint64_t l = 0;
        uint16_t c = 0;
        uint64_t addr=0;
        collect_labels();
        for (uint64_t i = 0; i < code.size();)
        {
            uint8_t s = code[i];
            if (s == ';')
            {
                i++;
                while (i<code.size()&&code[i] != '\n')
                    i++;
            }
            else if (i < code.size()&&s == '\n')
            {
                l++;
                c=0;
                i++;
            }
            else if (s == '\t' || s == '\r' || s == ' ')
            {
                c++;
                i++;
            }
            else if (is_letter(s)||s=='_')
            {
                std::string id;
                while (i < code.size() && (is_letter(code[i]) || is_int(code[i]) || code[i] == '_' || code[i] == ':'))
                {
                    id.push_back(code[i]);
                    i++;
                    c++;
                }
                if(!id.empty() && id.back() == ':') {
                    id.pop_back();
                    continue;
                }
                if((id[0]=='R'||id[0]=='r')&&std::all_of(id.begin()+1, id.end(),::isdigit)) {
                    std::string_view n(id.data()+1,id.size()-1);
                    lexed.emplace_back(token{REGN, l, c, std::string{n}, addr});
                    addr++;
                    continue;
                }
                if(id=="include") {
                    while(i<code.size()&&code[i]!='\n') {
                        i++;
                        c++;
                    }
                    continue;
                }
                std::string opcode = "0";
                if(id=="ld") opcode = "0x01";
                if(id=="add") opcode = "0x02";
                if(id=="sub") opcode = "0x03";
                if(id=="mul") opcode = "0x04";
                if(id=="div") opcode = "0x05";
                if(id=="imul") opcode = "0x06";
                if(id=="idiv") opcode = "0x07";
                if(id=="xor") opcode = "0x08";
                if(id=="and") opcode = "0x09";
                if(id=="or") opcode = "0x0A";
                if(id=="shl") opcode = "0x0B";
                if(id=="shr") opcode = "0x0C";
                if(id=="jmp") opcode = "0x0D";
                if(id=="cmp") opcode = "0x0E";
                if(id=="jz") opcode = "0x0F";
                if(id=="jnz") opcode = "0x10";
                if(id=="jc") opcode = "0x11";
                if(id=="jnc") opcode = "0x12";
                if(id=="store") opcode = "0x13";
                if(id=="ldm") opcode = "0x14";
                if(id=="jl") opcode = "0x15";
                if(id=="jle") opcode = "0x16";
                if(id=="jb") opcode = "0x17";
                if(id=="jbe") opcode = "0x18";
                if(id=="jmprv") opcode = "0x19";
                if(id=="push") opcode = "0x1A";
                if(id=="pop") opcode = "0x1B";
                if(id=="call") opcode = "0x1C";
                if(id=="ret") opcode = "0x1D";
                if(id=="fadd") opcode = "0x1E";
                if(id=="fsub") opcode = "0x1F";
                if(id=="fmul") opcode = "0x20";
                if(id=="fdiv") opcode = "0x21";
                /*
                if(id=="ADD") opcode = "1";
                if(id=="ADD") opcode = "1";
                if(id=="ADD") opcode = "1";
                if(id=="ADD") opcode = "1";
                if(id=="ADD") opcode = "1";
                */
                if(id=="hlt") opcode = "0xFF";
                if(opcode=="0"&&!labels.contains(id)) {
                    throw assembly_error("[Error - assembly:" + filename + std::to_string(l) + ':' + std::to_string(c) + "]: unknown instruction '" + id + "' found in code\n");
                }
                if(opcode=="0") {
                    lexed.emplace_back(token{LABEL, l, c, id, addr});
                    addr+=8;
                    continue;
                }
                lexed.emplace_back(token{ID, l, c, opcode, addr});
                addr++;
            } else if (is_int(s)) {
                std::string number;
                while ((i < code.size() && is_int(code[i])))
                {
                    number.push_back(code[i]);
                    i++;
                    c++;
                }
                lexed.emplace_back(token{INT, l, c, number, addr});
                addr+=8;
            }
            else if (s == ',')
            {
                lexed.emplace_back(token{COMA, l, c, ""});
                c++;
                i++;
            }
            else
            {
                i++;
                c++;
            }
        }
        lexed.emplace_back(token{EOF_, l, c, "", addr});
    }
};

class assembly {
    private:
    std::vector<token> lexed;
    std::array<uint8_t, 8> slice64(uint64_t i) {
        std::array<uint8_t, 8> res;
        std::memcpy(res.data(), &i, sizeof(i));
        return res;
    }
    std::array<uint8_t, 4> slice32(uint32_t i) {
        std::array<uint8_t, 4> res;
        std::memcpy(res.data(), &i, sizeof(i));
        return res;
    }
    std::array<uint8_t, 2> slice16(uint16_t i) {
        std::array<uint8_t, 2> res;
        std::memcpy(res.data(), &i, sizeof(i));
        return res;
    }
    uint64_t indx=0;
    void consume() {
        indx++;
    }
    token& peek() {
        if(indx<lexed.size()) return lexed[indx];
        else return lexed.back();
    }
    public:
    std::string file = "";
    void analyze() {
        for(uint64_t i=0;i<lexed.size()-1;i++) {
            token c = lexed[i];
            token n = lexed[i+1];
            if(c.t==ID&&n.t==ID) {
                throw assembly_error("[Error - assembly:" + file + std::to_string(c.line) + ':' + std::to_string(c.c) + "]: expected register or immediate after'" + c.val + "', but got '" + n.val + "'\n");
            }
        }
    }

    void init(const std::string &filename) {
        file = filename;
        std::ifstream f(filename);
        if(!f.is_open()) throw assembly_error("[Error - assembly]: file '" + filename + "' doesnt exitst\n");
        std::string line;
        std::string code;
        while(std::getline(f, line)) {
            code += line + '\n';
        }
        lexer l(code);
        l.lex();
        lexed = l.lexed;
    }

    std::vector<uint8_t> compile() {
        std::vector<uint8_t> compiled;
        analyze();
        while(indx<lexed.size()&&peek().t!=EOF_) {
            if(lexed[indx].t==ID) {
                std::string id = lexed[indx].val;
                compiled.emplace_back(std::stoul(id, 0, 16));
                consume();
                continue;
            } else if(peek().t==INT) {
                uint64_t val = std::stoull(lexed[indx].val);
                consume();
                std::array<uint8_t, 8> bytes = slice64(val);
                for(auto &x : bytes) {
                    compiled.emplace_back(x);
                }
            } else if(peek().t==REGN) {
                compiled.emplace_back(std::stol(lexed[indx].val)%256);
                consume();
            } else if(peek().t==LABEL) {
                uint64_t val = labels[peek().val];
                consume();
                std::array<uint8_t, 8> bytes = slice64(val);
                for(auto &x : bytes) {
                    compiled.emplace_back(x);
                }
            } else {
                consume();
            }
        }
        return compiled;
    }
};

#endif