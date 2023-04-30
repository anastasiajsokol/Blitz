#include <stack>

#include "blitz/error.h"
#include "blitz/io.h"

struct brackets {
  size_t id;
  std::stack<size_t> stack;

  brackets() : id(0) {}

  size_t reserve(){
    stack.push(id + 1);
    return ++id;
  }

  size_t pop(){
    size_t ret = stack.top();
    stack.pop();
    return ret;
  }

  size_t size(){
    return stack.size();
  }
};

void compile(blitz::inputfile input, blitz::outputfile output){
  const char* header =
    "[bits 64]\n"
    "global _start\n"
    "section .text\n"
    "extern putchar\n"
    "extern getchar\n"
    "extern fflush\n"
    "extern stdout\n"
    "_start:\n"
    "\txor rbx, rbx\n"  // use rbx for offset as it is callee saved
    "\tmov rbp, mem\n"; // use rbp for base as it is callee saved
  
  const char* footer =
    "\tmov rdi, [stdout]\n"
    "\tcall fflush\n"
    "\txor rdi, rdi\n"
    "\tmov dil, byte [rbp + rbx]\n"
    "\tmov rax, 60\n"
    "\tsyscall\n"
    "section .bss\n"
    "mem: resb 1024\n";
  
  output.writes(header);

  brackets jump_stack;
  size_t ident = 1;

  blitz::instruction inst; 
  while((inst = input.read()).id != EOF){
    switch(inst.id){
      case '+':
        output.format_write_line(ident, "add byte [rbp + rbx], %ld", inst.number);
        break;

      case '-':
        output.format_write_line(ident, "sub byte [rbp + rbx], %ld", inst.number);
        break;

      case '>':
        output.format_write_line(ident, "add rbx, %ld", inst.number); 
        break;
           
      case '<':
        output.format_write_line(ident, "sub rbx, %ld", inst.number); 
        break;

      case '[':
        while(inst.number--){
          size_t id = jump_stack.reserve();
          output.format_write_line(ident, ".bracket_%ld:", id);
          ++ident;
          output.format_write_line(ident, "cmp byte [rbp + rbx], 0");
          output.format_write_line(ident, "jz .end_bracket_%ld", id);
        }
        break;
        
      case ']':
        if(jump_stack.size() < inst.number){
          blitz::compile_error("Unmatched bracket", input.name, inst.line, inst.character);
        }
        while(inst.number--){
          size_t id = jump_stack.pop();
          output.format_write_line(ident, "jmp .bracket_%ld", id);
          --ident;
          output.format_write_line(ident, ".end_bracket_%ld:", id);
        }
        break;

      case '.':
        while(inst.number--){
          output.format_write_line(ident, "xor rdi, rdi");
          output.format_write_line(ident, "mov dil, byte [rbp + rbx]");
          output.format_write_line(ident, "call putchar");
        }
        break;

      case ',':
        while(inst.number--){
          output.format_write_line(ident, "call getchar");
        }
        output.format_write_line(ident, "mov byte [rbp + rbx], ax");
        break;
    }
  }

  output.writes(footer);
}

int main(int argc, char *argv[]){
    if(argc != 3){
        blitz::error("Blitz Compiler v0.1 Useage\n\targuments: $(input file) $(output file)\n");
    }

    FILE *input = fopen(argv[1], "r");
    if(input == NULL){
        blitz::error("Unable to open provided input file %s for reading\n", argv[1]);
    }

    FILE *output = fopen(argv[2], "w");
    if(output == NULL){
        blitz::error("Unable to open provided output file %s for writing\n", argv[2]);
    }

    compile(blitz::inputfile(input, argv[1]), blitz::outputfile(output, argv[2]));

    return 0;
}
