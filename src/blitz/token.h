#ifndef BLITZ_TOKEN
#define BLITZ_TOKEN

#include <list>
#include <ctype.h>

namespace blitz {

enum tokentype {
    // required
    add,
    sub,
    shiftr,
    shiftl,
    loop,
    input,
    output,
};

struct fposition {
    std::size_t line;
    std::size_t character;
};

struct token {
    tokentype type;
    
    std::size_t modifier;
    std::size_t offset;

    fposition position;

    std::list<token> children;

    token(tokentype type, std::size_t line, std::size_t character) : type(type), modifier(0), offset(0), position(fposition{line, character}) {}
    token(tokentype type, std::list<token> children, std::size_t line, std::size_t character) : type(type), modifier(0), offset(0), children(children), position(fposition{line, character}) {}
    token(tokentype type, size_t modifier, std::size_t line, std::size_t character) : type(type), modifier(modifier), offset(0), position(fposition{line, character}) {}
    token(tokentype type, size_t modifier, size_t offset, std::size_t line, std::size_t character) : type(type), modifier(modifier), offset(offset), position(fposition{line, character}) {}
};

typedef std::list<token> tokenset;

};

#endif