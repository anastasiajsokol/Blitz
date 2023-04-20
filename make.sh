#!/bin/sh

set -e

g++ compiler.cpp -o compiler
./compiler example/example.bf example/example.s
nasm -f elf64 example/example.s
ld example/example.o -o example/example
rm example/example.o
./example/example
