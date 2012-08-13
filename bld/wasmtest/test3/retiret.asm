	name	retiret

.386

.MODEL MEDIUM,C

.CODE

;.8086

public proc1
proc1 proc uses bx, param1:word, param2:word
      local x1:word, x2:word
	mov bx,param1
	mov x1,bx
	mov bx,param2
	mov x2,bx
	mov bx,param1
	mov param2,bx
	or   ax,ax
	jnz SHORT label1
	iret

label1:	or   ax,ax
	jnz SHORT label2
.386
	iretd

label2:	or   ax,ax
	jnz SHORT label3
	iretf	; ignore epilog code

label3:	or   ax,ax
	jnz SHORT label4
	iretdf	; ignore epilog code

label4:	or   ax,ax
	jnz SHORT label5
	ret

label5:	or   ax,ax
	jnz SHORT label6
	retn	; ignore epilog code

label6:	or   ax,ax
	jnz SHORT label7
	retf	; ignore epilog code

label7:	ret

proc1 endp

.DATA

x	db	0

END
