	include ..\cwlib.inc
	include ..\..\cw.inc
	scode

;-------------------------------------------------------------------------
;
;Release some memory.
;
;On Entry:
;
;ESI	- Address to release.
;
;On Exit:
;
;All registers preserved.
;
Free	proc	near
	pushm	eax,esi
	add	esi,DataLinearBase	;make it a linear address again.
	sys	RelMemLinear32
	popm	eax,esi
	ret
Free	endp


	efile
	end

