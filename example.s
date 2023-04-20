[bits 64]
global _start
section .text
_start:
	xor rsi, rsi
	mov rdx, mem
	mov rax, 60
	xor rdi, rdi
	mov dil, [rdx + rsi]
	syscall
section .bss
mem: resb 1000
