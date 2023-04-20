#!/bin/sh

set -e

g++ compiler.cpp -o compiler
./compiler example.bf example.s
nasm -f elf64 example.s
ld example.o -o example
rm example.o
./example
