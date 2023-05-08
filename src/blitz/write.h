#ifndef BLITZ_WRITE
#define BLITZ_WRITE

#include <stdio.h>

#include "token.h"
#include "error.h"

namespace blitz {

void _format_write_line(FILE *file, size_t indent, const char* format_string, ...){
    va_list args;
    va_start(args, format_string);
    while(indent--){ fputc('\t', file); }
    vfprintf(file, format_string, args);
    fputc('\n', file);
    va_end(args);
}

bool write(FILE *outfile, tokenset tokens){
    size_t indent = 0;
    for(auto& token : tokens){
        switch(token.type){
            case tokentype::add:
                _format_write_line(outfile, indent, "add byte [rbp + rbx], %ld", token.modifier);
                break;
            
            case tokentype::sub:
                _format_write_line(outfile, indent, "sub byte [rbp + rbx], %ld", token.modifier);
                break;
            
            case tokentype::shiftr:
                _format_write_line(outfile, indent, "add rbx, %ld", token.modifier);
                break;
            
            case tokentype::shiftl:
                _format_write_line(outfile, indent, "sub rbx, %ld", token.modifier);
                break;

            default:
                _format_write_line(outfile, indent, "Unknown [%d] {modifier = %ld, offset = %ld, empty = %s}", token.type, token.modifier, token.offset, token.children.empty() ? "true" : "false");
        }
    }

    return true;
}

};

#endif