; gostbox.asm
; 2021-02-07  Markku-Juhani O. Saarinen <mjos@mjos.fi>

; Small DOS .COM assembler program that generates and outputs the
; "Nonlinear bijection" S-Box defined for GOST R 34.11-2012 "Streebog"
; and GOST R 34.12-2015 "Kuznyechik."

; Syntax may be a bit odd (historic);
; Assemble with "nasm -f bin -o gostbox.com gostbox.asm"
; Run with dosbox or similar.

        org     0x100           ; .com file relocation start point
pi      equ     0x200           ; Pi table will go here

; ----  generate the table  pi[1..255]

start:  mov     bx,     pi      ; address
        mov     ax,     0xFCEE  ; pi[0] and pi[1] values
        mov     [bx],   ah
        inc     bl              ; bl = 0x01

        mov     di,     tab_s   ; sigma table

.for_i: mov     si,     tab_k   ; kappa table
        mov     ah,     [di]    ; y = sy[i]

.for_j: mov     [bx],   al
        mov     al,     [si]
        xor     al,     ah
        add     bl,     bl      ; left shift, high bit into carry
        jnc     .lfsr
        xor     bl,     0x1D    ; reduce by base polynomial
.lfsr:
        inc     si
        cmp     si,     tab_k + 17
        jnz     .for_j

        mov     al,     15[di]  ; tab_k is at offset 15 from tab_s
        inc     di
        cmp     di,     tab_s + 15
        jnz     .for_i

; ----- print pi

        dec     bl              ; set bx = pi again
.sout:
        mov     dl,     [bx]
        mov     ah,     0x02    ; DOS: "print character"
        int     0x21

        inc     bl
        jnz     .sout

        mov     ah,     0x4C    ; DOS: "terminate program"
        int     0x21

; ----- tables

tab_s:  db      0x01, 0xDD, 0x92, 0x4F, 0x93,
        db      0x99, 0x0B, 0x44, 0xD6, 0xD7,
        db      0x4E, 0xDC, 0x98, 0x0A, 0x45

tab_k:  db      0xDC, 0xCE, 0xFA, 0xE8, 0xF8, 0xEA, 0xDE, 0xCC,
        db      0xEC, 0xFE, 0xCA, 0xD8, 0xC8, 0xDA, 0xEE, 0xFC

