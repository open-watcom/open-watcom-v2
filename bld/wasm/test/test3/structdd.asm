.model small

.data
testx	struc
	ts32_eip	dd	?
	ts32_cs		dw	?
testx	ends

.code
.386
	mov es:[di].testx.ts32_cs, cs
	mov es:[di].testx.ts32_eip, offset exec_continue
	mov ax, cs
	xor ax,cx

exec_continue:
	pop bp
	ret

end