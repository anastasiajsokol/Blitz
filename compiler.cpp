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

void compile(FILE *input, FILE *output){
    const char* header =
        "[bits 64]\n"
        "global _start\n"
        "section .text\n"
        "_start:\n"
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