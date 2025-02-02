global _start
_start:
    mov rax, 10
    push rax
    push QWORD [rsp + 0]

    push QWORD [rsp + 8]

    mov rax, 60
    pop rdi
    syscall