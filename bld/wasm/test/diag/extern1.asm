
;--- type conflict: externdefs - usage

        .386
        .model flat

		.data

externdef var1:word

var1	dword 0

		.code

main    proc c

		mov eax,var1
        ret

main    endp

        END


