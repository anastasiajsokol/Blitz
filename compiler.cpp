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
    "global main\n"    // prefer '_start' when c standard libraries are not needed
    "section .text\n"
    "extern putchar\n" // replace later with proper port handling, maybe
    "extern getchar\n" // replace later with proper port handling, maybe
    "main:\n"          // prefer '_start' when c standard libraries are not needed
    "\txor rsi, rsi\n"
    "\tmov rdx, mem\n";
  
  const char* footer =
    "\tmov rax, 60\n"
    "\txor rdi, rdi\n"
    "\tmov dil, [rdx + rsi]\n"
    "\tsyscall\n"
    "section .bss\n"
    "mem: resb 1000\n";
  
  fputs(header, output);
  
  size_t jump_stack = 0;
  
  instruction inst; 
  while((inst = read_from_file(input)).id != EOF){
    switch(inst.id){
      case '+':
        format_write(output, jump_stack + 1, "add byte [rdx + rsi], %ld", inst.n);        
        break;

      case '-':
        format_write(output, jump_stack + 1, "sub byte [rdx + rsi], %ld", inst.n);
        break;

      case '>':
        format_write(output, jump_stack + 1, "add rsi, %ld", inst.n); 
        break;
           
      case '<':
        format_write(output, jump_stack + 1, "sub rsi, %ld", inst.n); 
        break;

      case '[':
        while(inst.n--){
          ++jump_stack;
          format_write(output, jump_stack + 0, ".bracket_%ld:", jump_stack);
        }
        break;
        
      case ']':
        while(inst.n--){
          format_write(output, jump_stack + 1, "cmp byte [rdx + rsi], 0");
          format_write(output, jump_stack + 1, "jz .bracket_%ld", jump_stack);
          --jump_stack;
        }
        break;

      case '.':
        format_write(output, jump_stack + 1, "mov rax, [rdx + rsi]");
        while(inst.n--){
          format_write(output, jump_stack + 1, "call putchar");
        }
        break;

      case ',':
        while(inst.n--){
          format_write(output, jump_stack + 1, "call getchar");
        }
        format_write(output, jump_stack + 1, "mov byte [rdx + rsi], ax");
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
