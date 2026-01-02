bits 16
cpu 8086
mov ax, 10
mov bx, 10
mov cx, 10
.L9:
cmp bx, cx
je .L18
add ax, 1
jp .L23
.L18:
sub bx, 5
jb .L26
.L23:
sub cx, 2
.L26:
loopnz .L9
