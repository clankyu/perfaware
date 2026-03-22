global mov_all_bytes_asm
global nop_all_bytes_asm
global cmp_all_bytes_asm:
global dec_all_bytes_asm:

section .text

mov_all_bytes_asm:
    xor rax, rax
.loop:
    mov [rdx + rax], al
    inc rax
    cmp rax, rcx
    jb .loop
    ret

nop_all_bytes_asm:
    xor rax, rax
.loop:
    db 0x0f, 0x1f, 0x00
    inc rax
    cmp rax, rcx
    jb .loop
    ret

cmp_all_bytes_asm:
    xor rax, rax
.loop:
    inc rax
    cmp rax, rcx
    jb .loop
    ret

dec_all_bytes_asm:
    xor rax, rax
.loop:
    dec rcx
    jnz .loop
    ret

