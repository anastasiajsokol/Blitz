#ifndef BLITZ_SCAN
#define BLITZ_SCAN

#include <list>
#include <stdio.h>

#include "token.h"
#include "error.h"

namespace blitz {

tokenset _scantokens(FILE *input, const char* input_name, bool expect_closing_bracket, std::size_t line, std::size_t character){
    std::list<token> tokens;

    while(true){
        ++character;
        switch(fgetc(input)){
            case '+':
                tokens.push_back(token(tokentype::add, 1, line, character));
                break;
            
            case '-':
                tokens.push_back(token(tokentype::sub, 1, line, character));
                break;
            
            case '>':
                tokens.push_back(token(tokentype::shiftr, 1, line, character));
                break;
            
            case '<':
                tokens.push_back(token(tokentype::shiftl, 1, line, character));
                break;
            
            case ',':
                tokens.push_back(token(tokentype::input, 1, line, character));
                break;
            
            case '.':
                tokens.push_back(token(tokentype::output, 1, line, character));
                break;
            
            case '[':
                tokens.push_back(token(tokentype::loop, _scantokens(input, input_name, true, line, character), line, character));
                break;
            
            case '\n':
                ++line;
                character = 0;
                break;

            case ']':
                if(!expect_closing_bracket){
                    blitz::compile_error("Unmatched closing bracket", input_name, line, character);
                }
                goto _end;
                
            case EOF:
                if(expect_closing_bracket){
                    blitz::compile_error("Expected closing bracket but instead found end of file", input_name, line, character);
                }
                goto _end;
        }
    }

    _end:
    
    return tokens;
}

tokenset scantokens(FILE *input, const char* input_name){
    return _scantokens(input, input_name, false, 1, 0);
}

};

#endif