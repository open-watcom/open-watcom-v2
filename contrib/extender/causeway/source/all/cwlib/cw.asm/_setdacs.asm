	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Set the value's in VGA colour DAC's.
;
;Calling:
;
;SetDACs(base,count,palette);
;
;On Entry:
;
;base	- Starting DAC number.
;count	- Number of DAC's to set.
;palette	- Pallet information
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
_SetDACs	proc	syscall base:dword, count:dword, palette:dword
	public _SetDACs
	pushm	ebx,ecx,esi
	mov	ebx,base
	mov	ecx,count
	mov	esi,palette
	call	SetDACs
	popm	ebx,ecx,esi
	ret
_SetDACs	endp


	efile
	end


