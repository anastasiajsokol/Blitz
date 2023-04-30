#ifndef BLITZ_OPTIMIZER
#define BLITZ_OPTIMIZER

#include <queue>

#include "error.h"
#include "io.h"

namespace blitz {

enum instructionset {
    // brainf*ck required
    add,
    sub,
    shiftr,
    shiftl,
    open,
    close,
    in,
    out,

    // parsing extensions
    end,
    invalid,

    // optimization extensions
    set,
};

struct xinstruction {
    instructionset type;
    size_t modifier;
    fpos pos;
    
    xinstruction() : type(invalid), modifier(0), pos(fpos{0, 0}) {}
    xinstruction(instructionset type, size_t modifier, fpos pos) : type(type), modifier(modifier), pos(pos) {} 
};

class optimizer {
    private:
        inputfile& input;
        std::queue<xinstruction> wait;
        
    public:
        optimizer(inputfile& input) : input(input) {}
        
        xinstruction read(){
            if(!wait.empty()){
                xinstruction top = wait.front();
                wait.pop();
                return top;
            }

            instruction base = input.read();

            switch(base.id){
                case '+':
                    return xinstruction(add, base.number, base.pos);
                
                case '-':
                    return xinstruction(sub, base.number, base.pos);
                
                case '>':
                    return xinstruction(shiftr, base.number, base.pos);
                
                case '<':
                    return xinstruction(shiftl, base.number, base.pos);
                
                case '[':
                    {
                        instruction next = input.read();
                        instruction last = input.read();
                        
                        if((next.id == '-' || next.id == '+') && next.number == 1 && last.id == ']'){
                            if(base.number >= last.number){
                                if(base.number > last.number){
                                    wait.push(xinstruction(open, base.number - last.number, base.pos));
                                }

                                instruction lookahead = input.read();

                                if(lookahead.id == '+'){
                                    wait.push(xinstruction(set, lookahead.number % 256, last.pos));
                                } else if(lookahead.id == '-'){
                                    wait.push(xinstruction(set, 256 - lookahead.number % 256, last.pos));
                                } else {
                                    input.unread(lookahead);
                                    wait.push(xinstruction(set, 0, last.pos));
                                }

                                xinstruction top = wait.front();
                                wait.pop();
                                return top;
                            } else {
                                // could be used to create an 'if' statement, but for now do not optimize
                                wait.push(xinstruction(close, last.number - base.number, last.pos));
                                return xinstruction(set, 0, next.pos);
                            }
                        }

                        input.unread(last);
                        input.unread(next);

                        return xinstruction(open, base.number, base.pos);
                    }
                
                case ']':
                    return xinstruction(close, base.number, base.pos);
                
                case ',':
                    return xinstruction(in, base.number, base.pos);
                    
                case '.':
                    return xinstruction(out, base.number, base.pos);
                
                case EOF:
                    return xinstruction(end, base.number, base.pos);
                
                [[unlikely]] default:
                    compile_error("Invalid instruction in optimizer (likely compiler error)", input.name, base.pos.line, base.pos.character);
            }
        }
};

};

#endif