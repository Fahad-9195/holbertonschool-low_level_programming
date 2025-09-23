; 100-hello_world.asm
; Prints "Hello, World" + newline using the write syscall, then exits.

global _start

section .data
msg:    db  "Hello, World", 10   ; 10 = '\n'
len:    equ $ - msg

section .text
_start:
    ; write(stdout=1, msg, len)
    mov     rax, 1          ; sys_write
    mov     rdi, 1          ; fd = 1 (stdout)
    mov     rsi, msg        ; buf
    mov     rdx, len        ; count
    syscall

    ; exit(0)
    mov     rax, 60         ; sys_exit
    xor     rdi, rdi        ; status = 0
    syscall

