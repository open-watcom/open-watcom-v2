	.386p

                NAME    kbhit

                EXTRN   "C", __cbyte :BYTE

DGROUP          GROUP   CONST,CONST2,_DATA,_BSS

_TEXT           SEGMENT BYTE PUBLIC USE32 'CODE'
                ASSUME  CS:_TEXT ,DS:DGROUP,SS:DGROUP

                PUBLIC  "C",kbhit_

kbhit_	proc	near
	cmp	dword ptr __cbyte,00000000H
	jz	L1
	mov	eax,00000001H
	ret     

L1:	mov	ax,ds:[41ah]
	cmp	ax,ds:[41ch]
	jnz	L2
	xor	eax,eax
	ret

L2:	mov	ah,0bH
	int	21H
	cbw     
	cwde    
	ret     
kbhit_	endp

_TEXT           ENDS

CONST           SEGMENT DWORD PUBLIC USE32 'DATA'
CONST           ENDS

CONST2          SEGMENT DWORD PUBLIC USE32 'DATA'
CONST2          ENDS

_DATA           SEGMENT DWORD PUBLIC USE32 'DATA'
_DATA           ENDS

_BSS            SEGMENT DWORD PUBLIC USE32 'BSS'
_BSS            ENDS

                END
