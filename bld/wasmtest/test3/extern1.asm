
        .386
        .model flat
;		option casemap:none

externdef proc1:near	;public
externdef proc2:near	;external
externdef proc3:near	;not used

		.code

proc1	proc
		ret
proc1	endp

main    proc c

		call proc1
		call proc2
        ret

main    endp

        END


