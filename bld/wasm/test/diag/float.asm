.386

DATA	SEGMENT use16 'DATA'

xValue:    dd       -0.2125, -0.7154, -0.0721, -0.735
xValue1:    dw	0
xValue2:    dt       -0.2125, -0.7154, -0.0721

DATA ENDS

TEXT16	SEGMENT use16 'CODE'

assume	ds:DATA

	mov	esi,-0.735
	mov	ebx,-0.735
	mov	[esi],-0.735
	mov	[ebx],-0.735
	mov	dword ptr [esi],-0.735
	mov	dword ptr [ebx],-0.735
	mov	xValue,-0.735
	mov	xValue + 4,-0.735
	mov	xValue [ebx] + 4,-0.735
	mov	xValue [bx] + 4,-0.735
	mov	[si],-0.735
	mov	[bx],-0.735
	mov	dword ptr [si],-0.735
	mov	dword ptr [bx],-0.735
	push	-0.735
	or	esi,-0.735
;	mov	byte ptr [esi],-0.735
;	mov	byte ptr [ebx],-0.735
;	mov	word ptr [esi],-0.735
;	mov	word ptr [ebx],-0.735
;	mov	si,-0.735
;	mov	bx,-0.735
;	mov	byte ptr [si],-0.735
;	mov	byte ptr [bx],-0.735
;	mov	word ptr [si],-0.735
;	mov	word ptr [bx],-0.735
;	mov	xValue1,-0.735
;	mov	xValue1 + 4,-0.735
;	mov	xValue1 [bx] + 4,-0.735
;	mov	al,-0.735
;	mov	bl,-0.735
;	or	si,-0.735

TEXT16	ENDS

TEXT32	SEGMENT use32 'CODE'

assume	ds:DATA

	mov	esi,-0.735
	mov	ebx,-0.735
	mov	[esi],-0.735
	mov	[ebx],-0.735
	mov	dword ptr [esi],-0.735
	mov	dword ptr [ebx],-0.735
	mov	xValue,-0.735
	mov	xValue + 4,-0.735
	mov	xValue [ebx] + 4,-0.735
	mov	xValue [bx] + 4,-0.735
	mov	[si],-0.735
	mov	[bx],-0.735
	mov	dword ptr [si],-0.735
	mov	dword ptr [bx],-0.735
	push	-0.735
	or	esi,-0.735
;	mov	byte ptr [esi],-0.735
;	mov	byte ptr [ebx],-0.735
;	mov	word ptr [esi],-0.735
;	mov	word ptr [ebx],-0.735
;	mov	si,-0.735
;	mov	bx,-0.735
;	mov	byte ptr [si],-0.735
;	mov	byte ptr [bx],-0.735
;	mov	word ptr [si],-0.735
;	mov	word ptr [bx],-0.735
;	mov	xValue1,-0.735
;	mov	xValue1 + 4,-0.735
;	mov	xValue1 [bx] + 4,-0.735
;	mov	al,-0.735
;	mov	bl,-0.735
;	or	si,-0.735

TEXT32	ENDS

end
