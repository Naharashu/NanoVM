#ifndef NANOVM_ASSEMBLY_H
#define NANOVM_ASSEMBLY_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#include <array>

enum tok_type : uint8_t
{
    ID,
    COMA,
    SEMI,
    NEWLINE,
    INT,
    LONG_INT,
    EOF_
};

typedef struct token
{
    tok_type t;
    uint64_t line;
    uint16_t c;
    std::string val = "";
} token;


class lexer
{
public:
    std::string code = "";
    std::vector<token> lexed;
    lexer(const std::string &c) : code(c) {};

    inline bool is_int(unsigned char c)
    {
        return c >= '0' && c <= '9';
    }

    inline bool is_letter(unsigned char c)
    {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }

    void lex()
    {
        uint64_t l = 0;
        uint16_t c = 0;
        for (uint64_t i = 0; i < code.size();)
        {
            uint8_t s = code[i];
            if (s == ';')
            {
                i++;
                while (code[i] != '\n')
                    i++;
                l++;
                c++;
                i++;
            }
            else if (s == '\n')
            {
                l++;
                c++;
                i++;
            }
            else if (s == '\t' || s == '\r' || c == ' ')
            {
                c++;
            }
            else if (is_letter(s))
            {
                std::string id;
                while (i < code.size() && (is_letter(code[i]) || is_int(code[i]) || code[i] == '_'))
                {
                    id.push_back(code[i]);
                    i++;
                    c++;
                }
                lexed.push_back(token{ID, l, c, id});
            }
            else if (is_int(s))
            {
                std::string number;
                while ((i < code.size() && is_int(code[i])))
                {
                    number[i] = code[i];
                }
                char* endptr;
                uint64_t val = strtoull(number.c_str(), &endptr, 0);
                if(val < UINT8_MAX) lexed.push_back(token{INT, l, c, number});
                else lexed.push_back(token{LONG_INT, l, c, number});
            }
            else if (s == ',')
            {
                lexed.push_back(token{COMA, l, c, ""});
                c++;
                i++;
            }
            else
            {
                lexed.push_back(token{EOF_, l, c, ""});
            }
        }
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
    uint64_t indx=0;
    void consume() {
        indx++;
    }
    token peek() {
        if(indx+1<lexed.size()) return lexed[indx+1];
        else token{EOF_, 0,0,""};
    }
    public:
    void init(std::ifstream f) {
        std::string line;
        std::string code;
        while(std::getline(f, line)) {
            code += line;
        }
        lexer l(code);
        l.lex();
        lexed = l.lexed;
    }

    std::vector<uint8_t> compile() {
        std::vector<uint8_t> compiled;
        while(indx<lexed.size()) {
            if(lexed[indx].t==ID) {
                //compiled.push_back(lexed[indx].val);
                if(peek().t==INT) {
                    
                }
            }
        }
    }
};

#endif