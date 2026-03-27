global Read_x1_asm
global Read_x2_asm
global Read_x3_asm
global Read_x4_asm

section .text

Read_x1_asm:
	align 64
.loop:
    mov rax, [rdx]
    sub rcx, 1
    jnle .loop
    ret

Read_x2_asm:
	align 64
.loop:
    mov rax, [rdx]
    mov rax, [rdx]
    sub rcx, 2
    jnle .loop
    ret

Read_x3_asm:
    align 64
.loop:
    mov rax, [rdx]
    mov rax, [rdx]
    mov rax, [rdx]
    sub rcx, 3
    jnle .loop
    ret

Read_x4_asm:
	align 64
.loop:
    mov rax, [rdx]
    mov rax, [rdx]
    mov rax, [rdx]
    mov rax, [rdx]
    sub rcx, 4
    jnle .loop
    ret
