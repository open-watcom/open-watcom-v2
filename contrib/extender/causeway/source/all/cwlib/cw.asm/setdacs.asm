	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Set the value's in VGA colour DAC's.
;
;On Entry:
;
;EBX	- Starting DAC number.
;ECX	- Number of DAC's to set.
;ESI	- Pallet information
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
SetDACs	proc	near
	pushm	eax,ecx,edx,esi
	mov	edx,03c8h
	mov	al,bl
	out	dx,al
	inc	edx
	mov	eax,ecx
	shl	ecx,1
	add	ecx,eax
	cld
	rep	outsb
	popm	eax,ecx,edx,esi
	ret
SetDACs	endp


	efile
	end

