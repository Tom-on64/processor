; Computes Nth fibbonacci number and leaves it in d

@org 0x0000
@define N 10

;
; 0, 1, 1, 2, 3, 5, 8, ...
;

start:
    mov c, (N-1)
    mov a, 0
    mov b, 1
.loop:
    mov d, a
    add d, b
    mov a, b
    mov b, d
    sub c, 1
    lda .loop
    jnz c

    inb z, 0x00
    or z, 0x01
    outb z, 0x00

;
; Hand assembler :)
;
; _symbols:
;   start       0x0000
;   start.loop  0x0006
;
; _text:
; (start)
; 00: 0A 0A
; 02: 08 00
; 04: 09 01
; (start.loop)
; 06: 03 00
; 08: 93 01
; 0A: 00 01
; 0C: 01 03
; 0E: FA 01
; 10: 58 06 00
; 13: 62
; 14: 7c 00
; 16: cc 01
; 18: 8c 00
;

