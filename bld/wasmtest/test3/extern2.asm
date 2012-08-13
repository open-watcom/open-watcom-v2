
;--- two externdefs identical name (MASM accepts this)

        .386
        .model flat

		.data

externdef var1:dword
externdef var1:dword

		.code

main    proc c

		mov eax,var1
        ret

main    endp

        END


