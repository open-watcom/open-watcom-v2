.386

_DATA16 SEGMENT USE16 'DATA'

data_dd16 dd ?

          dw _DATA16
          dw _DATA32
          dw _TEXT16
          dw _TEXT32
          dw SEG data_dd16
          dw SEG code_proc16
          dw SEG data_dd32
          dw SEG code_proc32

          dd _DATA16
          dd _DATA32
          dd _TEXT16
          dd _TEXT32
          dd SEG data_dd16
          dd SEG code_proc16
          dd SEG data_dd32
          dd SEG code_proc32

_DATA16 ENDS

_DATA32 SEGMENT USE32 'DATA'

data_dd32 dd ?

          dw _DATA16
          dw _DATA32
          dw _TEXT16
          dw _TEXT32
          dw SEG data_dd16
          dw SEG code_proc16
          dw SEG data_dd32
          dw SEG code_proc32

          dd _DATA16
          dd _DATA32
          dd _TEXT16
          dd _TEXT32
          dd SEG data_dd16
          dd SEG code_proc16
          dd SEG data_dd32
          dd SEG code_proc32

_DATA32 ENDS

_TEXT16 SEGMENT USE16 'CODE'

        assume cs:_TEXT16

public code_proc16

code_proc16 proc

            mov ax,_DATA16
            mov ax,_DATA32
            mov eax,_DATA16
            mov eax,_DATA32

            mov dword ptr [bx],_DATA16
            mov dword ptr [bx],_DATA32
            mov dword ptr [ebx],_DATA16
            mov dword ptr [ebx],_DATA32

        assume ds:_DATA16
            mov ax,SEG data_dd16
            mov eax,SEG data_dd16
            mov eax,SEG code_proc16

            mov dword ptr [bx],SEG data_dd16
            mov dword ptr [ebx],SEG data_dd16
            mov dword ptr [bx],SEG code_proc16
            mov dword ptr [ebx],SEG code_proc16

        assume ds:_DATA32
            mov ax,SEG data_dd32
            mov eax,SEG data_dd32
            mov eax,SEG code_proc32

            mov dword ptr [bx],SEG data_dd32
            mov dword ptr [ebx],SEG data_dd32
            mov dword ptr [bx],SEG code_proc32
            mov dword ptr [ebx],SEG code_proc32

            ret

code_proc16 endp

_TEXT16 ENDS

_TEXT32 SEGMENT USE32 'CODE'

        assume cs:_TEXT32

public code_proc32

code_proc32 proc

            mov ax,_DATA16
            mov ax,_DATA32
            mov eax,_DATA16
            mov eax,_DATA32

            mov dword ptr [bx],_DATA16
            mov dword ptr [bx],_DATA32
            mov dword ptr [ebx],_DATA16
            mov dword ptr [ebx],_DATA32

        assume ds:_DATA16
            mov ax,SEG data_dd16
            mov eax,SEG data_dd16
            mov eax,SEG code_proc16

            mov dword ptr [bx],SEG data_dd16
            mov dword ptr [ebx],SEG data_dd16
            mov dword ptr [bx],SEG code_proc16
            mov dword ptr [ebx],SEG code_proc16

        assume ds:_DATA32
            mov ax,SEG data_dd32
            mov eax,SEG data_dd32
            mov eax,SEG code_proc32

            mov dword ptr [bx],SEG data_dd32
            mov dword ptr [ebx],SEG data_dd32
            mov dword ptr [bx],SEG code_proc32
            mov dword ptr [ebx],SEG code_proc32

            ret

code_proc32 endp

_TEXT32 ENDS


end

