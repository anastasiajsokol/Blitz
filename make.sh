#!/bin/sh

set -e

g++ compiler.cpp -o compiler
./compiler example/example.bf example/example.s
nasm -f elf64 example/example.s
ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o example/example example/example.o -lc
rm example/example.o
./example/example
