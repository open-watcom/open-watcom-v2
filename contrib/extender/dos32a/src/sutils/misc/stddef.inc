
cr	equ 0Dh,0Ah
cre	equ 0Dh,0Ah,00h


bptr	equ byte ptr
wptr	equ word ptr
dptr	equ dword ptr
offs	equ offset
fptr	equ far ptr

clr	macro r1
	xor r1,r1
	endm

rdtsc	macro
	db 0Fh, 31h
	endm

TRUE	equ 1
FALSE	equ 0
