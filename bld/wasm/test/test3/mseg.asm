; Test multiple segment type

; default use is use16 and CPU 8086

code segment 'CODE'	; 16-bit segment

assume	cs:code, ds:code

	add     al, '@' - '9'
	add     al, '@' - 4
	mov ax, '01'

.386	; set 386 CPU

	mov eax, 'kybo'

code ends

; default use is use16
; CPU is 386

code1 segment 'CODE'	; 16-bit segment

assume	cs:code1, ds:code1

	add     al, '@' - '9'
	add     al, '@' - 4
	mov ax, '01'
	mov eax, 'kybo'

code1 ends

.386	; change default use to use32 and CPU to 386

code2 segment 'CODE'	; 32-bit segment

assume	cs:code2, ds:code2

	add     al, '@' - '9'
	add     al, '@' - 4
	mov ax, '01'
	mov eax, 'kybo'

code2 ends

.8086	; change default use to use16 and CPU to 8086

.model small

.code	; 16-bit segment

	mov al, 1

.386	; set 386 CPU

.data	; 16-bit segment

x	db	0

.code	; 16-bit segment

	mov eax, 1

end
