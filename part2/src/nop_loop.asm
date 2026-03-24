global mov_all_bytes_asm
global nop_all_bytes_asm
global cmp_all_bytes_asm
global dec_all_bytes_asm
global nop_1x1_all_bytes_asm
global nop_1x3_all_bytes_asm
global nop_1x9_all_bytes_asm
global conditional_nop_asm

section .text

align 16
mov_all_bytes_asm:
    xor rax, rax
.loop:
    mov [rdx + rax], al
    inc rax
    cmp rax, rcx
    jb .loop
    ret

align 16
nop_all_bytes_asm:
    xor rax, rax
.loop:
    db 0x0f, 0x1f, 0x00
    inc rax
    cmp rax, rcx
    jb .loop
    ret

align 16
cmp_all_bytes_asm:
    xor rax, rax
.loop:
    inc rax
    cmp rax, rcx
    jb .loop
    ret

align 16
dec_all_bytes_asm:
    xor rax, rax
.loop:
    dec rcx
    jnz .loop
    ret

align 16
nop_1x1_all_bytes_asm:
    xor rax, rax
.loop:
    nop
    inc rax
    cmp rax, rcx
    jb .loop
    ret

align 16
nop_1x3_all_bytes_asm:
    xor rax, rax
.loop:
    nop
    nop
    nop
    inc rax
    cmp rax, rcx
    jb .loop
    ret

align 16
nop_1x9_all_bytes_asm:
    xor rax, rax
.loop:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    inc rax
    cmp rax, rcx
    jb .loop
    ret

align 16
conditional_nop_asm:
    xor rax, rax
.loop:
    mov r10, [rdx + rax]
    inc rax
    test r10, 1
    jnz .skip
    nop
.skip:
    cmp rax, rcx
    jb .loop
    ret
