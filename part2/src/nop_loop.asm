global mov_all_bytes_asm
global nop_all_bytes_asm
global cmp_all_bytes_asm
global dec_all_bytes_asm
global nop_1x1_all_bytes_asm
global nop_1x3_all_bytes_asm
global nop_1x9_all_bytes_asm
global conditional_nop_asm
global nop_all_bytes_unaligned_63_asm

global Read_x1_asm
global Read_x1_backwards_asm
global Read_x2_asm
global Read_x3_asm
global Read_x4_asm

global read_4x3
global read_8x3
global read_16x3
global read_32x3
global read_32x4
global read_32x2
global read_32x1
global read_128_x_bits
global read_leftover_bits
global read_128_x_bits_unrolled
global read_256x8_alignment

section .text

mov_all_bytes_asm:
    xor rax, rax
align 64
.loop:
    mov [rdx + rax], al
    inc rax
    cmp rax, rcx
    jb .loop
    ret

nop_all_bytes_asm:
    xor rax, rax
align 64
.loop:
    db 0x0f, 0x1f, 0x00
    inc rax
    cmp rax, rcx
    jb .loop
    ret

nop_all_bytes_unaligned_63_asm:
    xor rax, rax
align 64
.loop:
%rep 63
    nop
%endrep
    db 0x0f, 0x1f, 0x00
    inc rax
    cmp rax, rcx
    jb .loop
    ret

cmp_all_bytes_asm:
    xor rax, rax
align 64
.loop:
    inc rax
    cmp rax, rcx
    jb .loop
    ret

dec_all_bytes_asm:
    xor rax, rax
align 64
.loop:
    dec rcx
    jnz .loop
    ret

nop_1x1_all_bytes_asm:
    xor rax, rax
align 64
.loop:
    nop
    inc rax
    cmp rax, rcx
    jb .loop
    ret

nop_1x3_all_bytes_asm:
    xor rax, rax
align 64
.loop:
    nop
    nop
    nop
    inc rax
    cmp rax, rcx
    jb .loop
    ret

nop_1x9_all_bytes_asm:
    xor rax, rax
align 64
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

conditional_nop_asm:
    xor rax, rax
align 64
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

Read_x1_asm:
	align 64
.loop:
    mov rax, [rdx]
    sub rcx, 1
    jnle .loop
    ret

Read_x1_backwards_asm:
    sub rcx, 1
	align 64
.loop:
    mov rax, [rdx + rcx]
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

read_4x3:
    xor rax, rax
    align 64
.loop:
    mov r8d, [rdx]
    mov r8d, [rdx + 4]
    mov r8d, [rdx + 8]
    add rax, 12
    cmp rax, rcx
    jb .loop
    ret

read_8x3:
    xor rax, rax
    align 64
.loop:
    mov r8, [rdx]
    mov r8, [rdx + 8]
    mov r8, [rdx + 16]
    add rax, 24
    cmp rax, rcx
    jb .loop
    ret

read_16x3:
    xor rax, rax
    align 64
.loop:
    vmovdqu xmm0, [rdx]
    vmovdqu xmm0, [rdx + 16]
    vmovdqu xmm0, [rdx + 32]
    add rax, 48
    cmp rax, rcx
    jb .loop
    ret

read_32x3:
    xor rax, rax
    align 64
.loop:
    vmovdqu ymm0, [rdx]
    vmovdqu ymm0, [rdx + 32]
    vmovdqu ymm0, [rdx + 64]
    add rax, 96
    cmp rax, rcx
    jb .loop
    ret

read_32x2:
    xor rax, rax
    align 64
.loop:
    vmovdqu ymm0, [rdx]
    vmovdqu ymm0, [rdx + 32]
    add rax, 64
    cmp rax, rcx
    jb .loop
    ret

read_32x4:
    xor rax, rax
    align 64
.loop:
    vmovdqu ymm0, [rdx]
    vmovdqu ymm0, [rdx + 32]
    vmovdqu ymm0, [rdx + 64]
    vmovdqu ymm0, [rdx + 96]
    add rax, 128
    cmp rax, rcx
    jb .loop
    ret

read_32x1:
    xor rax, rax
    align 64
.loop:
    vmovdqu ymm0, [rdx]
    add rax, 32
    cmp rax, rcx
    jb .loop
    ret

read_128_x_bits:
    align 64
.outer:
    mov rax, rdx
    mov r9, r8
.inner:
    vmovdqu ymm0, [rax]
    vmovdqu ymm0, [rax + 0x20]
    vmovdqu ymm0, [rax + 0x40]
    vmovdqu ymm0, [rax + 0x60]
    vmovdqu ymm0, [rax + 0x80]
    vmovdqu ymm0, [rax + 0xa0]
    vmovdqu ymm0, [rax + 0xc0]
    vmovdqu ymm0, [rax + 0xe0]

    add rax, 256
    dec r9
    jnz .inner

    dec rcx
    jnz .outer

    ret

read_128_x_bits_unrolled:
    align 64
.outer:
    mov rax, rdx
    mov r9, r8
.inner:
    vmovdqu ymm0, [rax]
    vmovdqu ymm1, [rax + 0x20]
    vmovdqu ymm2, [rax + 0x40]
    vmovdqu ymm3, [rax + 0x60]
    vmovdqu ymm4, [rax + 0x80]
    vmovdqu ymm5, [rax + 0xa0]
    vmovdqu ymm6, [rax + 0xc0]
    vmovdqu ymm7, [rax + 0xe0]

    add rax, 256
    dec r9
    jnz .inner

    dec rcx
    jnz .outer

    ret

read_leftover_bits:
    jrcxz .is_zero
    xor r9, r9
.loop:
    mov rax, [rdx + r9]
    inc r9
    cmp rcx, r9
    jnz .loop

.is_zero:
    ret

read_256x8_alignment:
    mov rax, rdx
    add rax, r8
    align 64
.loop:
    vmovdqu ymm0, [rax]
    vmovdqu ymm0, [rax + 0x20]
    vmovdqu ymm0, [rax + 0x40]
    vmovdqu ymm0, [rax + 0x60]
    vmovdqu ymm0, [rax + 0x80]
    vmovdqu ymm0, [rax + 0xa0]
    vmovdqu ymm0, [rax + 0xc0]
    vmovdqu ymm0, [rax + 0xe0]
    add rax, 256
    sub rcx, 256
    jnz .loop

    ret
