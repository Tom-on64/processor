; A very basic test program
; Written to be hand assembled, because i don't have an assembler yet

@org 0x0000

start:
    mov a, 10
    mov b, 8
    add a, b
    stw [out], a
    inb z, 0x00 ; Get status register values
    or z, 0x01 ; Write 1 to halt bit
    outb z, 0x00

out: db 0

;
; Hand Assembler Output:
;
; start = 0x0000
; out = 0x000F
;
; 0x00: 08 0A
; 0x02: 09 08
; 0x04: A0 01
; 0x06: 28 0F 00
; 0x09: 7C 00
; 0x0B: CC 01
; 0x0D: 8C 00
; 0x0F: 00
;

