.386

_DATA16 SEGMENT USE16 'DATA'

	dd offset data_dd32
	dd data_dd32
	df data_dd32

	dd offset data_dd16
	dd data_dd16

	dd offset code_proc32
	dd code_proc32
	df code_proc32

	dd offset code_proc16
	dd code_proc16

data_dd16 dd ?

_DATA16 ENDS

_DATA32 SEGMENT USE32 'DATA'

	dd offset data_dd32
	dd data_dd32
	df data_dd32

	dd offset data_dd16
	dd data_dd16

	dd offset code_proc32
	dd code_proc32
	df code_proc32

	dd offset code_proc16
	dd code_proc16
	dd 0

data_dd32 dd ?

_DATA32 ENDS

_TEXT16 SEGMENT USE16 'CODE'

        assume cs:_TEXT16

            nop
            nop
            nop

public code_proc16

code_proc16 proc far

        assume ds:_DATA16
            mov ax,word ptr data_dd16
            mov ax,offset data_dd16
            mov eax,data_dd16
            mov eax,offset data_dd16
            mov eax,code_proc16
            mov eax,offset code_proc16

        assume ds:_DATA32
            mov ax,word ptr data_dd32
            mov ax,offset data_dd32
            mov eax,data_dd32
            mov eax,offset data_dd32
            mov eax,code_proc32
            mov eax,offset code_proc32
            ret

code_proc16 endp

_TEXT16 ENDS

_TEXT32 SEGMENT USE32 'CODE'

        assume cs:_TEXT32

            nop
            nop
            nop
            nop

public code_proc32

code_proc32 proc far

        assume ds:_DATA16
            mov ax,word ptr data_dd16
            mov ax,offset data_dd16
            mov eax,data_dd16
            mov eax,offset data_dd16
            mov eax,code_proc16
            mov eax,offset code_proc16

        assume ds:_DATA32
            mov ax,word ptr data_dd32
;            mov ax,offset data_dd32
            mov eax,data_dd32
            mov eax,offset data_dd32
            mov eax,code_proc32
            mov eax,offset code_proc32
            ret

code_proc32 endp

_TEXT32 ENDS


end

