	include ..\cwlib.inc
	scode


;------------------------------------------------------------------------------
;
;Convert character to upper case.
;
;On Entry:
;
;AL	- character code.
;
;On Exit:
;
;AL	- upper case character code.
;
;ALL other registers preserved.
;
UpperChar	proc	near
	cmp	al,61h		; 'a'
	jb	@@1
	cmp	al,7Ah		; 'z'
	ja	@@1
	and	al,5Fh		;convert to upper case.
@@1:	ret
UpperChar	endp


	efile
	end

