.386p
MP equ 10h
SW_ST equ 20h

_data segment use32 'DATA'

testx	struc
	ts32_eip	dd	?
	ts32_cs		dw	?
testx	ends

t_table struc
t_next   dw     0
t_prev   dw     0
t_stktop dw     0
t_regnum dw     0
         dw     0
t_table ends

_data ends


_TEXT           SEGMENT BYTE PUBLIC USE32 'CODE'
                ASSUME  CS:_TEXT

tentab  t_table <       10,     70,     00 shl 11, 0    >       ; 00

		mov     di,word ptr cs:tentab[edi].t_prev
		mov     bx,word ptr cs:tentab[edi].t_stktop
		and     word ptr ds:[ebp].t_prev,NOT SW_ST
		or      word ptr ds:[ebp].t_prev,bx
                mov     byte ptr es:[[ebx]],0CCH
		mov	ebx,[edx][[eax*2]]
		cmps	byte ptr [eax], byte ptr [[(eax)]]
		cmps	byte ptr cs:[eax],[[eax]]
		mov	es:[di].testx.ts32_cs, cs
		mov	es:[di].testx.ts32_eip, offset exec_continue
		mov	es:testx.ts32_cs[[di]], cs
		mov	es:testx.ts32_eip[[di]], offset exec_continue

		mov	ax,++(((1+2)+((-3*--4)))+5)+6-99

exec_continue:
                lea     dx,word ptr +2H[edx]
                mov     dx,word ptr es:+2H[bx]
                mov     dx,word ptr es:-2H[bx]
                lea     eax,-2H[edx]
                mov     eax,-2H[edx]
                lea     eax,+2H[edx]
                mov     eax,+2H[edx]
                mov     eax,es:-2H[edx]
                mov     eax,2H[edx]
                mov     eax,es:2H[edx]
                mov     eax,-2H[edx-2h]
                mov     eax,es:-2H[edx-2h]
                mov     eax,2H[edx-2h]
                mov     eax,es:2H[edx-2h]
                mov     ax,-52
		mov	ax,not MP
                mov     ax,-1
_TEXT           ENDS

                END
