#ifndef BLITZ_IO
#define BLITZ_IO

#include <stdarg.h>
#include <stdio.h>
#include <stack>

namespace blitz {

struct instruction {
    int id;
    size_t n;

    instruction(){}
    instruction(int id, size_t n) : id(id), n(n) {}
};

class inputfile {
    private:
        FILE *file;
        std::stack<instruction> reflow;
    
    public:
        inputfile(const inputfile&) = delete;
        inputfile(FILE *file) : file(file) {}
        ~inputfile(){ fclose(file); }
        
        instruction read(){
            #define is_instruction_id(c) (c == '+' | c == '-' | c == '>' | c == '<' | c == '[' | c == ']' | c == '.' | c == ',' | c == EOF)
            if(!reflow.empty()){
                instruction ret = reflow.top();
                reflow.pop();
                return ret;
            }
            int id;
            do {
                id = fgetc(file);
            } while(!is_instruction_id(id));
            if(id == EOF){
                return instruction(EOF, 0);
            }
            size_t n = 1;
            int c;
            do {
                c = fgetc(file);
                if(is_instruction_id(c)){
                    if(c != id){ break; }
                    ++n;
                }
            } while(true);
            ungetc(c, file);
            return instruction(id, n);
            #undef is_instruction_id
        }

        void unread(instruction unused){
            reflow.push(unused);
        }
};

class outputfile {
    private:
        FILE *file;

    public:
        outputfile(const outputfile&) = delete;
        outputfile(FILE *file) : file(file) {}
        ~outputfile(){ fclose(file); }

        int writes(const char* string){
            return fputs(string, file);
        }

        void format_write_line(size_t indent, const char* format_string, ...){
            va_list args;
            va_start(args, format_string);
            while(indent--){ fputc('\t', file); }
            vfprintf(file, format_string, args);
            fputc('\n', file);
            va_end(args);
        }
};

};

#endif