	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Read the command line tail for parameters.
;
;supports / or - or + as switch/option settings. Options and names may be in
;any order, text can be tagged onto options, names must be in correct sequence
;for current program.
;
;Each character ( 33 to 127) has an entry in OptionTable & OptionTable+128.
;
;The first entry is a byte, and is 0 for OFF & none-zero for ON.
;The second entry is a pointer to any additional text specified, 0 means none.
;Entries 0-32 are reserved for text not preceded by - or + or /. These are
;intended to be file names, but neadn't be.
;All text entries are 0 terminated.
;
;OptionCounter	 - Total command line parameters (files & switches).
;OptionTable	 - Switch table, ASCII code is index to check.
;OptionPointers - Pointer table, ASCII code*4 is index to use.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;nothing.
;
_ReadCommand	proc	syscall
	public _ReadCommand
	call	ReadCommand
	ret
_ReadCommand	endp


	efile
	end


