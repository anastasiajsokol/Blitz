#ifndef BLITZ_OPTIMIZER
#define BLITZ_OPTIMIZER

#include <utility>
#include <queue>
#include <list>

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
    addmul,
};

struct xinstruction {
    instructionset type;
    size_t modifier;
    size_t offset;
    fpos pos;

    xinstruction() : type(invalid), pos(fpos{0, 0}), modifier(0) {}
    xinstruction(instructionset type, size_t modifier, fpos pos) : type(type), pos(pos), modifier(modifier), offset(0) {}
    xinstruction(instructionset type, size_t modifier, size_t offset, fpos pos) : type(type), pos(pos), modifier(modifier), offset(offset) {}
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
                    return xinstruction(add, base.number % 256, base.pos);
                
                case '-':
                    return xinstruction(sub, base.number % 256, base.pos);
                
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
                        
                        if((next.id == '>' || next.id == '<') && last.id == '+'){
                            char go = next.id;
                            char back = (go == '>') ? '<' : '>';
                            
                            std::list<std::pair<instruction, instruction>> instructions;

                            size_t n = next.number;

                            instruction a, b;
                            while(true){
                                a = input.read();

                                if(a.id != go){
                                    if(a.id == back && a.number == n){
                                        b = input.read();

                                        if(b.id == '-' && b.number == 1){
                                            instruction ending = input.read();

                                            if(ending.id == ']'){ // addmul pattern
                                                size_t i = next.number;
                                                for(auto revit = instructions.rbegin(); revit != instructions.rend(); ++revit){
                                                    i += revit->first.number;
                                                    wait.push(xinstruction(addmul, revit->second.number, i, revit->second.pos));
                                                }
                                                wait.push(xinstruction(set, 0, instructions.back().second.pos));
                                                return xinstruction(addmul, last.number, next.number, last.pos);
                                            }

                                            input.unread(ending);
                                        }

                                        input.unread(b);
                                    }

                                    input.unread(a);

                                    for(auto& x : instructions){
                                        input.unread(x.second);
                                        input.unread(x.first);
                                    }
                                    break;
                                }

                                b = input.read();

                                if(b.id != '+'){
                                    input.unread(b);
                                    input.unread(a);
                                    for(auto& x : instructions){
                                        input.unread(x.second);
                                        input.unread(x.first);
                                    }
                                    break;
                                }

                                n += a.number;
                                instructions.push_front({a, b});
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