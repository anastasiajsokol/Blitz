#include <stdio.h>

#include "blitz/error.h"
#include "blitz/write.h"
#include "blitz/scan.h"

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

    blitz::write(output, blitz::scantokens(input, argv[1]));

    return 1;
}
