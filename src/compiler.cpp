#include <stack>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void compile_error(const char* error, const char* file, size_t line, size_t offset){
  fprintf(stderr, "%s\n\t%s (%ld:%ld)\n", error, file, line, offset);
  exit(1);
}

void error(const char* error, ...){
  va_list args;
  va_start(args, error);
  vfprintf(stderr, error, args);
  va_end(args);
  exit(1);
}

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
};

struct instruction {
  char id;
  size_t n;

  instruction() : id(-1), n(0) {}
  instruction(char id, size_t n) : id(id), n(n) {}
};

void format_write(FILE* output, size_t indent, const char* format_string, ...){
  va_list args;
  va_start(args, format_string);
  while(indent--){ fputc('\t', output); }
  vfprintf(output, format_string, args);
  fputc('\n', output);
  va_end(args);
}

instruction read_from_file(FILE *input){
  char id = fgetc(input);
  
  if(id == EOF){
    return instruction(EOF, 0);
  }
  
  size_t n = 1;
  char c;
  while((c = fgetc(input)) == id){
    ++n;
  }
  ungetc(c, input);
  return instruction(id, n);
}

void compile(FILE *input, FILE *output){
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
    "mem: resb 1000\n";
  
  fputs(header, output);
  
  brackets jump_stack;
  size_t ident = 1;

  instruction inst; 
  while((inst = read_from_file(input)).id != EOF){
    switch(inst.id){
      case '+':
        format_write(output, ident, "add byte [rbp + rbx], %ld", inst.n);        
        break;

      case '-':
        format_write(output, ident, "sub byte [rbp + rbx], %ld", inst.n);
        break;

      case '>':
        format_write(output, ident, "add rbx, %ld", inst.n); 
        break;
           
      case '<':
        format_write(output, ident, "sub rbx, %ld", inst.n); 
        break;

      case '[':
        while(inst.n--){
          format_write(output, ident, ".bracket_%ld:", jump_stack.reserve());
          ++ident;
        }
        break;
        
      case ']':
        while(inst.n--){
          format_write(output, ident, "cmp byte [rbp + rbx], 0");
          format_write(output, ident, "jnz .bracket_%ld", jump_stack.pop());
          --ident;
        }
        break;

      case '.':
        while(inst.n--){
          format_write(output, ident, "xor rdi, rdi");
          format_write(output, ident, "mov dil, byte [rbp + rbx]");
          format_write(output, ident, "call putchar");
        }
        break;

      case ',':
        while(inst.n--){
          format_write(output, ident, "call getchar");
        }
        format_write(output, ident, "mov byte [rbp + rbx], ax");
        break;
    }
  }

  fputs(footer, output);
}

int main(int argc, char *argv[]){
    if(argc != 3){
        error("Blitz Compiler v0.1 Useage\n\targuments: $(input file) $(output file)\n");
    }

    FILE *input = fopen(argv[1], "r");
    if(input == NULL){
        error("Unable to open provided input file %s for reading\n", argv[1]);
    }

    FILE *output = fopen(argv[2], "w");
    if(output == NULL){
        error("Unable to open provided output file %s for writing\n", argv[2]);
    }

    compile(input, output);

    return 0;
}