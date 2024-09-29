[bits 16]
[org 0x7c00]

KERNEL equ 0x7e00

start:
    mov al, '!'
    mov ah, 0xe
    int 0x10

    mov ah, 0
    int 0x13

    ; Read CHS address
    ; Load pointer
    mov ax, KERNEL
    mov bx, ax
    mov ax, 0
    mov es, ax

    ; Load reading arguments
    mov al, 62 ; Number of sectors to read
    mov ch, 0  ; Cylinder to read
    mov cl, 2  ; Starting sector to read from
    mov dh, 0  ; Head to read from
    mov dl, 0x80 ; primary hard disk

    mov ah, 2 ; BIOS code

    int 0x13

    jc mem_read_err ; carry set if error


    mov ax, 0xaaaa ; load to 0x15000
    mov bx, ax
    mov ax, 0
    mov es, ax

    ; Load reading arguments (for assets)
    mov al, 40 ; Number of sectors to read
    mov ch, 0  ; Cylinder to read
    mov cl, 1  ; Starting sector to read from
    mov dh, 1  ; Head to read from
    mov dl, 0x80 ; primary hard disk

    mov ah, 2 ; BIOS code

    int 0x13

    jc mem_read_err

    ; Set video mode to 320x200 8-bit color using BIOS
    mov ah, 0
    mov al, 0x13
    int 0x10

    ; Enter protected mode when no disk errors
    cli ; Clear interrupts
    xor ax, ax
    mov ds, ax ; clear ds
    lgdt [gdt_desc]

    mov eax, cr0
    or al, 1       ; set PE (Protection Enable) bit in CR0 (Control Register 0)
    mov cr0, eax

    ; Far-jump to code segment in order to clear instructions. Also reset the stack to 0x90000.

    jmp 0x08:Protected_Start
[bits 32]
Protected_Start:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov esp, 0x90000 ; set stack far away, inside the data segment
    call 0x8:KERNEL

    jmp $


; data structures and etc.
; Should not be ran as code
mem_read_error_ptr:
    db "ERROR Reading from disk [0x80]",0

mem_read_err:
    mov si, mem_read_error_ptr
    mov ah, 0xe
print_loop:
    lodsb
    cmp al, 0
    je endprt
    int 0x10
    jmp print_loop
endprt:
    jmp $

; Define a 32-bit protected mode GDT


gdt_begin:
    gdt_null:
        dq 0
    gdt_code:
        dw 0xffff     ; Limit
        dw 0x0000     ; Base addr
        db 0x00       ; Base addr
        db 0b10011010 ; Access byte
        db 0b11001111 ; Flags & Limit
        db 0x00       ; Base
    gdt_data:
        dw 0xffff     ; Limit
        dw 0x0000     ; Base addr
        db 0x00       ; Base addr
        db 0b10010010 ; Access byte
        db 0b11001111 ; Flags & Limit
        db 0x00       ; Base
gdt_end:

gdt_desc:
    dw gdt_end-gdt_begin
    dd gdt_begin

times 510-($-$$) db 0
dw 0xaa55

%macro drive_read 5
    pusha
    ; macro to read the primary drive to memory
    ; (1) cylinder, (2) head, (3) start, (4) #num, (5) BX buffer pointer

    ; Load pointer
    mov ax, %5
    mov bx, ax
    mov ax, 0
    mov es, ax

    ; Load reading arguments
    mov al, %4
    mov ch, %1
    mov cl, %3
    mov dh, %2
    mov dl, 0x80 ; primary hard disk

    int 0x13

    jc mem_read_err ; carry set if error

    popa
%endmacro

