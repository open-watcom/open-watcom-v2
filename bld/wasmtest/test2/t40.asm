.386

.model small

shifted_value EQU 0F0000000h

.code

   mov bx,shifted_value SHR 20

END
