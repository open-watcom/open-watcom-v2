	include ..\cwlib.inc
	scode

;-----------------------------------------------------------------------------
;
;Have a look for VESA INT 10h BIOS extension and if present install a patch to
;make protected mode access transparent.
;
;Calling:
;
;InitVESA();
;
;On Entry:
;
;nothing
;
;On Exit:
;
;Carry set on error and EAX=0 else,
;
;EAX	- Address of bank switch code. Always != 0.
;
;ALL other registers preserved.
;
_InitVESA	proc	syscall
	public _InitVESA
	call	InitVESA
	ret
_InitVESA	endp


;-----------------------------------------------------------------------------
;
;Release VESA BIOS patch if installed.
;
;Calling:
;
;RelVESA();
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
;Notes: This routine can safely be called even if InitVESA hasn't been
;       called or didn't install due to lack of VESA.
;
_RelVESA	proc	syscall
	public _RelVESA
	call	RelVESA
	ret
_RelVESA	endp


	efile
	end
