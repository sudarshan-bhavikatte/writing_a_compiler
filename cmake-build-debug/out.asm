global _start
_start:
    mov rax, 5
    push rax
    mov rax, 8
    push rax
    mov rax, 11
    push rax
    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    pop rbx
    add rax, rbx
    push rax
    mov rax, 6
    push rax
    push QWORD [rsp + 8]
    mov rax, 60
    pop rdi
    syscall
