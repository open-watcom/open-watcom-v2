.386p
                NAME    initauto
                EXTRN   __exit_with_msg_ :near
                EXTRN   __STACKLOW :near
DGROUP          GROUP   _DATA,XI
_TEXT           SEGMENT DWORD PUBLIC USE32 'CODE'
                ASSUME  CS:_TEXT ,DS:DGROUP,SS:DGROUP
                PUBLIC  __CHK
                PUBLIC  __GRO
                PUBLIC  __STK
                PUBLIC  __STqOVERFLOW_
L1:             mov     word ptr L3,ss
                ret
__CHK:          xchg    eax,dword ptr +4H[esp]
                push    eax
                call    near ptr __STK
                call    __STK
                pop     eax
                shr     eax,2
                push    ecx
                mov     ecx,eax
                push    edi
                mov     edi,esp
                sub     edi,ecx
                mov     eax,55555555h
                stosd
                pop     edi
                pop     ecx
                mov     eax,dword ptr +4H[esp]
                ret     0004H
__GRO:          ret     0004H
__STK:          cmp     eax,esp
                jae     short L2
                sub     eax,esp
                neg     eax
                cmp     eax,dword ptr __STACKLOW
                jbe     short L2
                ret
L2:             mov     ax,ss
                cmp     ax,word ptr L3
                je      short __STqOVERFLOW_
                ret
__STqOVERFLOW_: mov     eax,offset DGROUP:L4
                mov     edx,00000001H
                jmp     near ptr __exit_with_msg_
_TEXT           ENDS

_DATA           SEGMENT DWORD PUBLIC USE32 'DATA'
L3              DB      00H,00H
L4              DB      53H,74H,61H,63H,6bH,20H,4fH,76H
                DB      65H,72H,66H,6cH,6fH,77H,21H,0dH
                DB      0aH,00H
_DATA           ENDS

XI              SEGMENT WORD PUBLIC USE32 'DATA'
                DB      00H,20H
                DD      L1
XI              ENDS

                END
