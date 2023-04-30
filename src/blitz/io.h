#ifndef BLITZ_IO
#define BLITZ_IO

#include <stdarg.h>
#include <stdio.h>
#include <stack>

namespace blitz {

struct instruction {
    int id;
    size_t number;
    
    size_t line;
    size_t character;

    instruction(){}
    instruction(int id, size_t number, size_t line, size_t character) : id(id), number(number), line(line), character(character) {}
};

class inputfile {
    private:
        FILE *file;

        std::stack<instruction> reflow;

        size_t line;
        size_t character;
    
    public:
        const char* name;
        
        inputfile(const inputfile&) = delete;
        inputfile(FILE *file, const char* name) : file(file), name(name), line(1), character(0) {}
        ~inputfile(){ fclose(file); }
        
        instruction read(){
            #define is_instruction_id(c) (c == '+' | c == '-' | c == '>' | c == '<' | c == '[' | c == ']' | c == '.' | c == ',' | c == EOF)
            #define update_position(c) line += (c == '\n'); character = (character + 1) * (c != '\n')

            if(!reflow.empty()){
                instruction ret = reflow.top();
                reflow.pop();
                return ret;
            }
            int id;
            do {
                id = fgetc(file);
                update_position(id);
            } while(!is_instruction_id(id));
            if(id == EOF){
                return instruction(EOF, 0, line, character);
            }
            size_t n = 1;
            int c;
            do {
                c = fgetc(file);
                if(is_instruction_id(c)){
                    if(c != id){ break; }
                    ++n;
                }
                update_position(c);
            } while(true);
            ungetc(c, file);
            return instruction(id, n, line, character);
            #undef update_position
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
        const char* name;
        
        outputfile(const outputfile&) = delete;
        outputfile(FILE *file, const char* name) : file(file), name(name) {}
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