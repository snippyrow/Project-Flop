[bits 32]
[extern kmain]

[global div0]
[global doublefault]
[global kbd_stub]
[global pit_stub]

[extern isr_ghandler]
[extern kbd_ghandler]
[extern pit_ghandler]


kernel_begin:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov esp, 0x90000 ; set stack far away, inside the data segment

    call kmain
    jmp $

isr_stub:
    call isr_ghandler
    add esp, 8
    iret

div0:
    push byte 0
    push byte 0 ; int no
    jmp isr_stub

doublefault:
    push byte 0
    push byte 8 ; int no
    jmp isr_stub

kbd_stub:
    call kbd_ghandler
    iret

pit_stub:
    call pit_ghandler
    iret